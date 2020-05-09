GetVecXTypeSize := !(Type^ itY) -> int
{
	if itY == GTypeVec4f return 4
	if itY == GTypeVec3f return 3
	if itY == GTypeVec2f return 2
	if itY == GTypeQuantf return 4
	return 0
}

GetVecTypeX := !(int Siz) -> Type^
{
	switch Siz
	{
		case 4 return GTypeVec4f
		case 3 return GTypeVec3f
		case 2 return GTypeVec2f
	}
}

BuiltInTemplateVec4fGet := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "."
	
		cc := Queue.{Type^}() ; $temp
		cc.Push(null->{Type^})

		MyFuncType = GetFuncType(cc,null->{bool^},GTypeInt,false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int 
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts

		if pars.Size() != 1 return 255
		if pars[0].first != GTypeVec4f and pars[0].first != GTypeQuantf
			and pars[0].first != GTypeVec3f and pars[0].first != GTypeVec2f return 255
		if consts.Size() != 1 return 255
		if consts[0].GetValue() != "~str" return 255

		asNeedPre := consts[0]->{ObjStr^}
		asNeed := asNeedPre.GetString()

		if asNeed == "0" return 255
		if asNeed == "sum" return 0

		if StrSize(asNeed) in 1..4
		{
			for i : asNeed
			{
				if not (i in "xyzw0") return 255
			}
			return 0
		}

		return 255
	}
	CreateFuncPointer := virtual !(FuncInputBox itBox) -> TypeFunc^
	{
		asNeedPre := itBox.itConsts[0]->{ObjStr^}
		asNeed := asNeedPre.GetString()
		
		outT := Queue.{Type^}() ; $temp
		FType := MyFuncType
		
		for it : FType.ParsCount, par : itBox.itPars
		{
			if FType.Pars[it] == null
			{
				outT.Push(par.first)
			}else{
				outT.Push(FType.Pars[it])
			}
		}
		newRet := MyFuncType.RetType
		retRefArray := MyFuncType.ParsIsRef

		wantRefs := StrSize(asNeed) == 1 //and itBox.itPars[0].second
		if not itBox.itPars[0].second
			wantRefs = false

		if wantRefs
		{
			retRefArray = new bool[MyFuncType.ParsCount] ; $temp

			for i : MyFuncType.ParsCount
			{
				retRefArray[i] = MyFuncType.ParsIsRef[i] or itBox.itPars[i].second
			}
		}

		if MyFuncType.RetType == null and CopyRet != null
		{
			newRet = ParseType(CopyRet)
		}
		if itBox.itPars.Size() > FType.ParsCount
		{
			retRefArray = new bool[itBox.itPars.Size()] ; $temp 
			for i : itBox.itPars.Size()
				retRefArray[i] = MyFuncType.ParsIsRef[i]
			for it, i : itBox.itPars
			{
				if i < outT.Size() continue
				outT.Push(it.first)
			}
		}
		return GetFuncType(outT,retRefArray,newRet,wantRefs,false)
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ funct) -> BoxFunc^
	{
		asNeedPre := itBox.itConsts[0]->{ObjStr^}
		asNeed := asNeedPre.GetString()
		pars := ref itBox.itPars

		typName := pars[0].first.GetName()

		outType := pars[0].first

		if asNeed == "sum"
		{
			tSize := GetVecXTypeSize(pars[0].first)
			return new BuiltInFuncUno(".",pars[0].first,false,GTypeFloat,false, 
			"#0 = call float @llvm.experimental.vector.reduce.v2.fadd.f32.v"sbt + tSize +
			"f32(float zeroinitializer, "+ typName + " #1)")
		}
		
		newSize := StrSize(asNeed)

		if GetVecXTypeSize(outType) != newSize 
			outType = GetVecTypeX(newSize)

		switch  newSize
		{
		case 1:
			x := 0
			if asNeed == "y" x = 1
			if asNeed == "z" x = 2
			if asNeed == "w" x = 3

			if itBox.itPars[0].second
			{
				return new BuiltInFuncUno(".",pars[0].first,true,GTypeFloat,true, "#0 = getelementptr "sbt + typName 
					+ " ," + typName + "* #1, i32 0, i32 " + x + "\n")
			}
			
			return new BuiltInFuncUno(".",pars[0].first,false,GTypeFloat, "#0 = extractelement "sbt + typName + " #1, i32 " + x + "\n")
		case 2..4
			vecData := int[4]
			for i : newSize
			{
				if asNeed[i] == 'x' vecData[i] = 0
				if asNeed[i] == 'y' vecData[i] = 1
				if asNeed[i] == 'z' vecData[i] = 2
				if asNeed[i] == 'w' vecData[i] = 3
				if asNeed[i] == '0' vecData[i] = 4
			}
			cmdText := 	"#0 = shufflevector "sbt
					+ typName + " #1, " + typName + " zeroinitializer , <" + newSize + " x i32>  " + 
					//"<i32 " + vecData[0] + ",i32 " + vecData[1] + ",i32 " + vecData[2] + ",i32 " + vecData[3] + ">\n"
					"<"
			for i : newSize
			{
				if i != 0 cmdText << " , "
				cmdText << "i32 " << vecData[i]
			}
			cmdText << ">\n"
			return new BuiltInFuncUno(".",pars[0].first,false,outType,cmdText)
		}
		return null
	}
}
Vec4fFuncs := !() -> void
{
	F4T := GetType("vec4f")
	FT := GetType("float")
	F4N := F4T.GetName()

	Typs2 := Type^[4]
	Typs2[0] = GTypeVec4f 
	Typs2[1] = GTypeQuantf
	Typs2[2] = GTypeVec3f
	Typs2[3] = GTypeVec2f
	for it : Typs2
	{
		itName := it.GetName()
		AddBuiltInFunc( new BuiltInFuncBinar("+",it,false,it,false,it,"#0 = fadd "sbt + itName + " #1 , #2 #d\n"))
		AddBuiltInFunc( new BuiltInFuncBinar("-",it,false,it,false,it,"#0 = fsub "sbt + itName + " #1 , #2 #d\n"))
		AddBuiltInFunc( new BuiltInFuncBinar("/",it,false,it,false,it,"#0 = fdiv "sbt + itName + " #1 , #2 #d\n"))
		AddBuiltInFunc( new BuiltInFuncBinar("*",it,false,it,false,it,"#0 = fmul "sbt + itName + " #1 , #2 #d\n"))
		AddBuiltInFunc( new BuiltInFuncBinar("=",it,true,it,false,GTypeVoid,"store "sbt + itName + " #2 ," + itName + "* #1 #d\n"))

		AddBuiltInFunc(new BuiltInFuncBinar("+=",it,true,it,false,it,"#0pre = load "sbt + itName +" , "+ itName +"* #1 #d\n"
											+"#0 = fadd " + itName + " #2,#0pre\n"
											+"store "+ itName +" #0, "+itName+"* #1 #d\n"))
		AddBuiltInFunc(new BuiltInFuncBinar("-=",it,true,it,false,it,"#0pre = load "sbt + itName +" , "+ itName +"* #1 #d\n"
											+"#0 = fsub " + itName + " #0pre,#2\n"
											+"store "+ itName +" #0, "+itName+"* #1 #d\n"))
	
		tS := GetVecXTypeSize(it)
		
		assmMulToVal := "%PrePreVec## = insertelement "sbt + itName + " undef , float #2,i32 0 #d\n" +
				"%PreVec## = shufflevector " + itName + " %PrePreVec## , " + itName + " undef , <" + tS + " x i32> <"
		for i : tS
		{
			if i != 0 assmMulToVal << " , "
			assmMulToVal << "i32 0"
		}
		assmMulToVal << ">" << "#0 = fmul " << itName << " #1 , %PreVec## #d\n"
		AddBuiltInFunc( new BuiltInFuncBinar("*",it,false,GTypeFloat,false,it,assmMulToVal))

		assmDivToVal := "%PrePreVec## = insertelement "sbt + itName + " undef , float #2,i32 0 #d\n" +
				"%PreVec## = shufflevector " + itName + " %PrePreVec## , " + itName + " undef , <" + tS + " x i32> <"
		for i : tS
		{
			if i != 0 assmDivToVal << " , "
			assmDivToVal << "i32 0"
		}
		assmDivToVal << ">" <<
				"#0 = fdiv " << itName << " #1 , %PreVec## #d\n"
		AddBuiltInFunc( new BuiltInFuncBinar("/",it,false,GTypeFloat,false,it,assmDivToVal))

		AddBuiltInFunc( new BuiltInFuncTypeTimes(". this",FT,1,it,"#0 = insertelement "sbt + itName + " zeroinitializer, float #1,i32 0 #d\n"))
		AddBuiltInFunc( new BuiltInFuncTypeTimes(". this",FT,2,it,"%Pre3p## = insertelement "sbt + itName + " zeroinitializer, float #1,i32 0 #d\n" + 
									"#0 = insertelement " + itName + " %Pre3p##, float #2,i32 1\n"))

		if tS >=3 {
		AddBuiltInFunc( new BuiltInFuncTypeTimes(". this",FT,3,it,"%Pre3p## = insertelement "sbt + itName + " zeroinitializer, float #1,i32 0 #d\n" + 
									"%Pre2p## = insertelement " + itName + " %Pre3p##, float #2,i32 1\n"+
									"#0 = insertelement " + itName + " %Pre2p##, float #3,i32 2\n"))
		if tS >= 4 {
		AddBuiltInFunc( new BuiltInFuncTypeTimes(". this",FT,4,it,"%Pre3p## = insertelement "sbt + itName + " zeroinitializer, float #1,i32 0 #d\n" + 
									"%Pre2p## = insertelement " + itName + " %Pre3p##, float #2,i32 1\n"+
									"%Pre1p## = insertelement " + itName + " %Pre2p##, float #3,i32 2\n"+
									"#0	  = insertelement " + itName + " %Pre1p##, float #4,i32 3\n"))
		}}
	}

	Typs := Type^[4]
	Typs[0] = F4T
	Typs[1] = GetType("quantf")
	Typs[2] = GTypeVec3f
	Typs[3] = GTypeVec2f
	
	for NT : Typs, siz : ![4,4,3,2]
	{
		AddBuiltInFunc( new BuiltInFuncBinar("<+>",NT,false,NT,false,FT,"%Pre## = fmul "sbt + NT.GetName() + " #1 , #2\n" + 
			"#0 = call fast float @llvm.experimental.vector.reduce.fadd.f32.v" + siz + "f32(float undef,<"+ siz + " x float> %Pre##) #d\n"))

	}
}
