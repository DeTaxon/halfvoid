#import "BoxFunc.cp"
#import "BoxClass.cp"
#import "Type.cp"

BuiltInFunc := class extend BoxFunc
{
	ToExe := string
	IsSelfPre := bool

	IsAssembler := virtual !() -> bool
	{
		return true
	}
	CheckIsSelf := !() -> void
	{	
		AsmLine := ToExe

		i := 0
		while AsmLine[i] != 0
		{
			if AsmLine[i] == '#'
			{
				i += 1
				if AsmLine[i] == '^' IsSelfPre = true
			}else i += 1
		}

	}
}

BuiltInSuffix := class extend BuiltInFunc
{
	this := !(string Name, Type^ l, bool lRef,Type^ retV, string code) -> void
	{
		IsSuffix = true
		FuncName = Name
		OutputName = Name
		ToExe = code

		PP := Queue.{Type^}()
		PP.Push(l)

		IsRefs := bool[1]
		IsRefs[0] = lRef
		MyFuncType = GetFuncType(PP,IsRefs,retV,false,false)

		CheckIsSelf()
	}
}

BuiltInFuncZero := class extend BuiltInFunc
{
	this := !(string Name,Type^ retV, bool RRetRef,string code) -> void
	{
		FuncName = Name
		OutputName = Name
		ToExe = code
		IsRetRef = RRetRef

		PP := Queue.{Type^}()
		MyFuncType = GetFuncType(PP,null->{bool^},retV,RRetRef,false)

		CheckIsSelf()
	}
}

BuiltInFuncTypeTimes := class extend BuiltInFunc
{
	this := !(string Name,Type^ inType, int Count,Type^ retV,string code) -> void
	{
		FuncName = Name
		OutputName = Name
		ToExe = code

		PP := Queue.{Type^}()
		for Count PP.Push(inType)

		if Name == ". this" 
		{
			ItConsts.Push(new ObjType(retV)) 
		}

		MyFuncType = GetFuncType(PP,null->{bool^},retV,false,false)

		CheckIsSelf()
	}
}

BuiltInFuncUno := class extend BuiltInFunc
{
	this := !(string Name, Type^ l, bool lRef,Type^ retV,bool RRetRef, string code) -> void
	{
		Init(Name,l,lRef,retV,RRetRef,code)
	}
	this := !(string Name, Type^ l, bool lRef,Type^ retV, string code) -> void
	{
		Init(Name,l,lRef,retV,false,code)
	}
	Init := !(string Name, Type^ l, bool lRef,Type^ retV, bool RRetRef, string code) -> void
	{
		FuncName = Name
		OutputName = Name
		ToExe = code
		IsRetRef = RRetRef

		PP := Queue.{Type^}()
		PP.Push(l)

		IsRefs := bool[1]
		IsRefs[0] = lRef
		MyFuncType = GetFuncType(PP,IsRefs,retV,RRetRef,false)

		CheckIsSelf()
	}
}
BuiltInFuncBinar := class extend BuiltInFunc
{
	this := !(string Name, Type^ l, bool lRef, Type^ r, bool rRef,Type^ retV,bool retRef, string code) -> void
	{
		FuncName = Name
		OutputName = Name
		ToExe = code
		IsRetRef = retRef

		PP := Queue.{Type^}()
		PP.Push(l)
		PP.Push(r)

		IsRefs := bool[2]
		IsRefs[0] = lRef
		IsRefs[1] = rRef
		MyFuncType = GetFuncType(PP,IsRefs,retV,retRef,false)
		CheckIsSelf()
	}
	this := !(string Name, Type^ l, bool lRef, Type^ r, bool rRef,Type^ retV, string code) -> void
	{
		FuncName = Name
		OutputName = Name
		ToExe = code

		PP := Queue.{Type^}()
		PP.Push(l)
		PP.Push(r)

		IsRefs := bool[2]
		IsRefs[0] = lRef
		IsRefs[1] = rRef
		MyFuncType = GetFuncType(PP,IsRefs,retV,false,false)
		CheckIsSelf()
	}
}
BuiltInFuncMega := class extend BuiltInFunc
{
	this := !(string Name, TypeFunc^ fType, string code) -> void
	{
		FuncName = Name
		OutputName = Name
		ToExe = code

		MyFuncType = fType
		CheckIsSelf()
	}
}

BuiltInTemplateGetRef := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "&"
		OutputName = "error"
		emptType := Queue.{Type^}()
		emptType.Push(null->{Type^})
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		if itBox.itPars.Size() != 1 return 255
		return 0
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ funct) -> BoxFunc^
	{
		pars := ref itBox.itPars
		return new BuiltInFuncUno("&",pars[0].first,true,pars[0].first.GetPoint(), 
		"#0 = getelementptr "sbt + pars[0].first.GetName() + " , " + pars[0].first.GetName() + "* #1, i32 0\n")
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
BuiltInTemplatePoint := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "^"
		OutputName = "error"
		emptType := Queue.{Type^}()
		emptType.Push(null->{Type^})
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		if pars.Size() != 1 return 255
		if not pars[0].first is TypePoint return 255
		return 0
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ funct) -> BoxFunc^
	{
		pars := ref itBox.itPars
		return new BuiltInFuncUno("^",pars[0].first,false,pars[0].first.Base,true, 
		"#0 = getelementptr "sbt + pars[0].first.Base.GetName() + " , " + pars[0].first.GetName() + " #1, i32 0\n")
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
BuiltInTemplatePointArr := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "[]"
		OutputName = "error"
		emptType := Queue.{Type^}()
		emptType.Push(null->{Type^})
		emptType.Push(null->{Type^})
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		if pars.Size() != 2 return 255
		if not (pars[0].first is TypePoint or pars[0].first is TypeArr or  pars[0].first is TypeFatArr) return 255
		
		if not IsInt(pars[1].first) return 255
		return 0
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ funct) -> BoxFunc^
	{
		pars := ref itBox.itPars
		if pars[0].first is TypeArr
		{
			return new BuiltInFuncBinar("[]",pars[0].first,true,pars[1].first,false,pars[0].first.Base,true,
		"#0 = getelementptr "sbt + pars[0].first.GetName() + " , " + pars[0].first.GetName() + "* #1, i32 0, " + pars[1].first.GetName() + " #2\n")
		}
		return new BuiltInFuncBinar("[]",pars[0].first,false,pars[1].first,false,pars[0].first.Base,true,
		"#0 = getelementptr "sbt + pars[0].first.Base.GetName() + " , " + pars[0].first.GetName() + " #1, " + pars[1].first.GetName() + " #2\n")
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}

BuiltInPointAdd := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "+"
		OutputName = "error"
		emptType := Queue.{Type^}()
		emptType.Push(null->{Type^})
		emptType.Push(GetType("s64"))
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		if pars.Size() != 2 return 255
		//if pars[0].first.GetType() != "point" and pars[0].first.GetType() != "arr" and pars[0].first.GetType() != "fatarr" return 255
		if not (pars[0].first is TypePoint or pars[0].first is TypeArr or  pars[0].first is TypeFatArr) return 255
		return TypeCmp(pars[1].first,GetType("s64"))
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ funct) -> BoxFunc^
	{
		pars := ref itBox.itPars
		if pars[0].first.Base == GTypeVoid
		{
			return new BuiltInFuncBinar("-",pars[0].first,false,GetType("s64"),false,pars[0].first.Base.GetPoint(),false,
			"#0 = getelementptr i8 , i8* #1 , i64 #2\n")
		}

		return new BuiltInFuncBinar("-",pars[0].first,false,GetType("s64"),false,pars[0].first.Base.GetPoint(),false,
		"#0 = getelementptr "sbt + pars[0].first.Base.GetName() + " , " + pars[0].first.Base.GetName() + "* #1 ,i64 #2\n")
	}
}
BuiltInPointSub := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "-"
		OutputName = "error"
		emptType := Queue.{Type^}()
		emptType.Push(null->{Type^})
		emptType.Push(GetType("s64"))
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		if pars.Size() != 2 return 255
		if pars[0].first.GetType() != "point" and pars[0].first.GetType() != "arr" and pars[0].first.GetType() != "fatarr" return 255
		return TypeCmp(pars[1].first,GetType("s64"))
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ funct) -> BoxFunc^
	{
		pars := ref itBox.itPars
		if pars[0].first.Base == GTypeVoid
		{
			return new BuiltInFuncBinar("-",pars[0].first,false,GetType("s64"),false,pars[0].first.Base.GetPoint(),false,
			"%Pre## = sub i64 0,#2\n"sbt +
			"#0 = getelementptr i8 , i8* #1 , i64 %Pre##\n")
		}

		return new BuiltInFuncBinar("-",pars[0].first,false,GetType("s64"),false,pars[0].first.Base.GetPoint(),false,
		"%Pre## = sub i64 0,#2\n"sbt +
		"#0 = getelementptr " + pars[0].first.Base.GetName() + " , " + pars[0].first.Base.GetName() + "* #1 ,i64 %Pre##\n")
	}
}

BuiltInTemplateExcArr := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "->{}"
		OutputName = "error"
		emptType := Queue.{Type^}()
		emptType.Push(null->{Type^})
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		if pars.Size() != 1 return 255
		if not pars[0].first is TypeArr  return 255
		return 0
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ funct) -> BoxFunc^
	{
		pars := ref itBox.itPars
		return new BuiltInFuncUno("->{}",pars[0].first,true,pars[0].first.Base.GetPoint(),false,
		"#0 = getelementptr "sbt + pars[0].first.GetName() + " , " + pars[0].first.GetName() + "* #1, i32 0,i32 0\n")
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}

BuiltInFuncClassInfo := class extend BuiltInFunc
{
	toGet := TypeClass^
	toCheck := int
	this := !(TypeClass^ tp,int toC) -> void
	{
		toGet = tp
		toCheck = toC

		cc := Queue.{Type^}()

		MyFuncType = GetFuncType(cc,null->{bool^},GTypeInt,false,false)

		PostFuncs.Push(this&)
	}
	PostCreate := !() -> void
	{
		asBase := toGet->{Type^}
		if toCheck == 0 //TypeSize
		{
			ToExe = "%Pre## = getelementptr " + asBase.GetName() + ", " + asBase.GetName() + "* null ,i32 1\n"
			ToExe = ToExe + "#0 = ptrtoint " + asBase.GetName() + "* %Pre## to i32\n"
		}
		if toCheck == 1 //Align
		{
			ToExe = "#0 = add i32 " + asBase.GetAlign() + ",0\n"
		}
		if toCheck == 2 //FatTypeSize
		{
			ToExe = "%Pre## = getelementptr " + asBase.GetName() + ", " + asBase.GetName() + "* null ,i32 1\n"
			ToExe = ToExe + "#0 = ptrtoint " + asBase.GetName() + "* %Pre## to i32\n"
		}
	}
}
BuiltInTemplateVec4fGet := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "."
	
		cc := Queue.{Type^}()
		cc.Push(null->{Type^})

		MyFuncType = GetFuncType(cc,null->{bool^},GTypeInt,false,false)
	}
	GetPriority := virtual !(Queue.{Type^} pars,Queue.{Object^} consts) -> int 
	{
		if pars.Size() != 1 return 255
		if pars[0] != GetType("vec4f") and pars[0] != GetType("quantf") return 255
		if consts.Size() != 1 return 255
		if consts[0].GetValue() != "~str" return 255

		asNeedPre := consts[0]->{ObjStr^}
		asNeed := asNeedPre.GetString()

		if asNeed == "x" return 0
		if asNeed == "y" return 0
		if asNeed == "z" return 0
		if asNeed == "w" return 0


		if StrSize(asNeed) == 4
		{
			for i : asNeed
			{
				if not (i in "xyzw") return 255
			}
			return 0
		}

		return 255
	}
	GetNewFunc := virtual  !(Queue.{Type^} pars,Queue.{Object^} consts, TypeFunc^ funct) -> BoxFunc^
	{
		asNeedPre := consts[0]->{ObjStr^}
		asNeed := asNeedPre.GetString()
		
		if StrSize(asNeed) == 1
		{
			x := 0
			if asNeed == "y" x = 1
			if asNeed == "z" x = 2
			if asNeed == "w" x = 3
			
			return new BuiltInFuncUno(".",pars[0],false,GetType("float"), "#0 = extractelement <4 x float> #1, i32 " + x + "\n")
		}else{
			vecData := int[4]
			for i : 4
			{
				if asNeed[i] == 'x' vecData[i] = 0
				if asNeed[i] == 'y' vecData[i] = 1
				if asNeed[i] == 'z' vecData[i] = 2
				if asNeed[i] == 'w' vecData[i] = 3
			}
			return new BuiltInFuncUno(".",pars[0],false,pars[0],"#0 = shufflevector <4 x float> #1, <4 x float> undef , <4 x i32> "sbt +
					"<i32 " + vecData[0] + ",i32 " + vecData[1] + ",i32 " + vecData[2] + ",i32 " + vecData[3] + ">\n")
		}
		return null
	}
}

BuiltInTemplateTypeInfo := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "->"
		OutputName = "error"
		emptType := Queue.{Type^}()
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts
		if pars.Size() != 0 return 255
		if consts.Size() != 2 return 255
		if consts[0].GetValue() != "~type" return 255
		if consts[1].GetValue() != "~str" return 255

		asN := consts[1]->{ObjStr^}
		St := asN.GetString()

		if St == "TypeSize" return 0
		if St == "Align" return 0
		if St == "FatTypeSize" return 0
		return 255
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ funct) -> BoxFunc^
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts

		asN := consts[1]->{ObjStr^}
		St := asN.GetString()

		intT := GTypeInt

		itT := (((consts[0])->{ObjType^}).MyType)

		if itT is TypeStandart
		{
			itT2 := itT->{TypeStandart^}

			if St == "TypeSize" return new BuiltInFuncZero("->",intT,false,"#0 = add i32 "sbt + itT2.ItSize + ", 0 \n")
			if St == "Align" return new BuiltInFuncZero("->",intT,false,"#0 = add i32 "sbt + itT2.ItAlign + ", 0 \n")
			if St == "FatTypeSize" return new BuiltInFuncZero("->",intT,false,"#0 = add i32 "sbt + itT2.ItSize + ", 0 \n")
		}else
		if itT is TypePoint or itT is TypeFatArr
		{
			//WARN64:
			if St == "TypeSize" return new BuiltInFuncZero("->",intT,false,"#0 = add i32 8, 0 \n")
			if St == "Align" return new BuiltInFuncZero("->",intT,false,"#0 = add i32 8, 0 \n")
			if St == "FatTypeSize" return new BuiltInFuncZero("->",intT,false,"#0 = add i32 8, 0 \n")
		}else
		if itT is TypeClass
		{
			itT2 := itT->{TypeClass^}
			if St == "TypeSize" return new BuiltInFuncClassInfo(itT2,0)
			if St == "Align" return new BuiltInFuncClassInfo(itT2,1)
			if St == "FatTypeSize" return new BuiltInFuncClassInfo(itT2,2)
		}

		return null
		
		//return new BuiltInFuncUno("->{}",pars[0],true,pars[0].Base.GetPoint(),false,
		//"#0 = getelementptr " + pars[0].GetName() + " , " + pars[0].GetName() + "* #1, i32 0,i32 0\n")
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}

BuiltInTemplateExcFatArr := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "->{}"
		OutputName = "error"
		emptType := Queue.{Type^}()
		emptType.Push(null->{Type^})
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		return 255

		//if pars.Size() != 1 return 255
		//if pars[0].GetType() != "fatarr" return 255
		//if consts.Size() != 1 return 255
		//if consts[0].GetValue() != "~type" return 255 
		return 0
	}
	GetNewFunc := virtual  !(Queue.{Type^} pars,Queue.{Object^} consts, TypeFunc^ funct) -> BoxFunc^
	{
		//toToT := ((consts[0]) ->{ObjType^}).MyType
		toToT := pars[0].Base.GetPoint()
		return new BuiltInFuncUno("->{}",pars[0],false,pars[0].Base.GetPoint(),false,
		"#0 = bitcast "sbt + pars[0].GetName() +  "#1 to " + toToT.GetName() + "\n")
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
BuiltInTemplateCheckPoint := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "->{}"
		OutputName = "error"
		emptType := Queue.{Type^}()
		emptType.Push(null->{Type^})
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts
		if pars.Size() != 1 return 255
		if not (pars[0].first is TypeFatArr or pars[0].first is TypePoint) return 255
		if consts.Size() != 1 return 255
		if not consts[0] is ObjType return 255

		asNeed := consts[0]->{ObjType^}
		if asNeed.MyType != GTypeBool return 255
		
		return 0
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ funct) -> BoxFunc^
	{
		pars := ref itBox.itPars
		return new BuiltInFuncUno("->{}",pars[0].first,false,GTypeBool,false,
		"#0 = icmp ne "sbt + pars[0].first.GetName() +  "#1,null\n")
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
BuiltInTemplateSet := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "="
		OutputName = "error"

		emptType := Queue.{Type^}()
		emptType.Push(null->{Type^})
		emptType.Push(null->{Type^})
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		if pars.Size() != 2 return 255
		if not (pars[0].first is TypePoint or pars[0].first is TypeFatArr ) return 255
		if not (pars[1].first is TypePoint or pars[1].first is TypeFatArr or pars[1].first is TypeArr) return 255
		if pars[1].first.Base == GTypeVoid return 1
		if pars[0].first is TypePoint
		{
			if pars[0].first.Base == GTypeVoid return 1
			if pars[1].first.Base == GTypeVoid return 1
			if pars[1].first.Base == pars[0].first.Base return 0
			if pars[1].first is TypePoint return TypeCmp(pars[1].first,pars[0].first)
		}else
		{
			if pars[1].first.Base == pars[0].first.Base return 0
		}
		return 255
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ funct) -> BoxFunc^
	{
		pars := ref itBox.itPars
		if pars[0].first is TypePoint and pars[0].first != pars[1].first 
		{
			PreRet := BoxFunc^
			if pars[1].first is TypeArr
			{
			PreRet = new BuiltInFuncBinar("=",pars[0].first,true,pars[1].first,true,GTypeVoid, 
					"%TPre## = bitcast "sbt + pars[1].first.GetName() + "* #2 to " + pars[0].first.GetName() + "\n" +
					"store " + pars[0].first.GetName() + " %TPre##, " + pars[0].first.GetName() + "* #1\n")
			}else{
			PreRet = new BuiltInFuncBinar("=",pars[0].first,true,pars[1].first,false,GTypeVoid, 
					"%TPre## = bitcast "sbt + pars[1].first.GetName() + " #2 to " + pars[0].first.GetName() + "\n" +
					"store " + pars[0].first.GetName() + " %TPre##, " + pars[0].first.GetName() + "* #1\n")
			}
			return PreRet
		}
		if pars[0].first != pars[1].first
		{
			return new BuiltInFuncBinar("=",pars[0].first,true,pars[1].first,false,GTypeVoid, 
					"%TPre## = bitcast "sbt + pars[1].first.GetName() + " #2 to " + pars[0].first.GetName() + "\n" +
					"store " + pars[0].first.GetName() + " %TPre##, " + pars[0].first.GetName() + "* #1\n")
		}
		return new BuiltInFuncBinar("=",pars[0].first,true,pars[1].first,false,GTypeVoid, "store "sbt + pars[0].first.GetName() + " #2, " + pars[0].first.GetName() +"* #1\n")
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}


BuiltInTemplateFuncWrapper := class extend BoxTemplate
{
	ToClass := BoxClass^
	ToFunc := BoxFunc^

	this := !(BoxFunc^ toAdd,BoxClass^ toClass) -> void
	{
		FuncName = toAdd.FuncName
		OutputName = toAdd.OutputName

		ToClass = toClass
		ToFunc = toAdd

		emptType := Queue.{Type^}()
		parsC := toAdd.MyFuncType.ParsCount
		
		refs := null->{bool^}

		if parsC > 1 refs = new bool[parsC-1]

		PP := Queue.{Type^}()

		for parsC
		{
			if it != 0
			{
				refs[it] = toAdd.MyFuncType.ParsIsRef[it-1]
				PP.Push(toAdd.MyFuncType.Pars[it-1])
			}
		}

		MyFuncType = GetFuncType(PP,refs,toAdd.MyFuncType.RetType,toAdd.MyFuncType.RetRef,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		return ComputePriorFunc(MyFuncType,itBox)
	}

	GetNewFunc := virtual  !(Queue.{Type^} pars,Queue.{Object^} consts, TypeFunc^ fun) -> BoxFunc^
	{
		Name := string
		AsStrObj := (consts[0]->{ObjStr^})
		Name = (AsStrObj.GetString())

		CType := ((ToClass.ClassType)->{Type^})
		CTypeP := CType.GetPoint()

		preRetCode := ""

		if ToFunc.MyFuncType.RetType != GTypeVoid
			preRetCode = "#0 = "

		asBasePre := ToFunc.MyFuncType
		asBase := asBasePre->{Type^}
		preRetCode = preRetCode + asBase.GetName() + "@" + ToFunc.OutputName

		preRet :=  new BuiltInFuncMega(".",ToFunc.MyFuncType,preRetCode)
		return	preRet
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}

BuiltInTemplateAutoField := class extend BoxTemplate
{
	ToClass := BoxClass^

	this := !(BoxClass^ toAdd) -> void
	{
		FuncName = "."
		OutputName = "error"

		ToClass = toAdd

		emptType := Queue.{Type^}()
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts

		if pars.Size() != 0 return 255
		if consts.Size() != 1 return 255
		if (not consts[0] is ObjStr) return 255

		asStrT := (consts[0]->{ObjStr^})
		asStr := asStrT.GetString()

		
		for ToClass.Params.Size()
		{
			if ToClass.Params[it].ItName == asStr
				return 0			
		}
		for ToClass.FakeParams.Size()
		{
			if ToClass.FakeParams[it].ItName == asStr
				return 0
		}
		return 255
	}

	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ fun) -> BoxFunc^
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts

		Name := string
		if consts.Size() != 1 return null
		if not consts[0] is ObjStr
			return null
		AsStrObj := (consts[0]->{ObjStr^})
		Name = (AsStrObj.GetString())

		pos := -1
		for ToClass.Params.Size()
		{
			if ToClass.Params[it].ItName == Name
			{
				pos = it
			}
		}
		if pos == -1 
		{
			for ToClass.FakeParams.Size()
			{
				if ToClass.FakeParams[it].ItName == Name
				{
					pos = it
				}
			}
			if pos != -1
			{
				CType := ((ToClass.ClassType)->{Type^})
				CTypeP := CType.GetPoint()

				FP := ToClass.FakeParams[pos]
				if not FP.Atter is ObjIndent return null //TODO: check in GetPrior
				FPN := ((FP.Atter)->{ObjIndent^}).MyStr
				pos2 := -1
				midType := FieldParam^

				for ToClass.Params.Size()
				{
					if ToClass.Params[it].ItName == FPN {
						midType = ToClass.Params[it]
						pos2 = it
					}
				}
				if pos2 == -1 return null //TODO: check in GetPrior

				if ToClass.ContainVirtual pos2 += 1
				return  new BuiltInFuncZero(".",FP.ResultType,true,
				"%Pre## = getelementptr "sbt + (CType.GetName()) + " , " + (CTypeP.GetName()) + " %this, i32 0, i32 "+pos2+"\n" +
				"#0 = bitcast " + midType.ResultType.GetName() + "* %Pre## to " + FP.ResultType.GetName() + "*\n")
			}
			ErrorLog.Push("Cannot find field "+Name+"\n")
			return null
		}

		CType := ((ToClass.ClassType)->{Type^})
		CTypeP := CType.GetPoint()

		fatPos := pos
		if ToClass.ContainVirtual fatPos += 1
		itStr := "#0 = getelementptr "sbt+ (CType.GetName()) + " , " + (CTypeP.GetName()) + " %this, i32 0, i32 "+fatPos+"\n" 
		preRet :=  new BuiltInFuncZero(".",ToClass.Params[pos].ResultType,true,itStr)
		return preRet
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
BuiltInLambdaCall := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "()"
		IsMethod = true

		pars := Queue.{Type^}()

		MyFuncType = null
	}
	CreateFuncPointer := virtual !(FuncInputBox itBox) -> TypeFunc^
	{
		pars := ref itBox.itPars

		asL := pars[0].first->{TypeFuncLambda^}
		asB :=  ((pars[0].first.Base)->{TypeFunc^})

		Pars := Queue.{Type^}()

		for i : pars.Size()
		{
			if i == 0{
				Pars.Push(pars[0].first)
			}else{
				Pars.Push(asB.Pars[i])
			}
		}
		return GetFuncType(Pars,asB.ParsIsRef,asB.RetType,asB.RetRef,asB.IsVArgs)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars

		if pars.Size() == 0 return 255
		if not pars[0].first is TypeFuncLambda return 255

		asL := pars[0].first->{TypeFuncLambda^}
		asB := ((pars[0].first.Base)->{TypeFunc^})

		if asB.ParsCount != pars.Size() return 255

		maxCmp := 0
		for i : pars.Size()
		{
			if i > 0
			{
				nowCmp := TypeCmp(pars[i].first,asB.Pars[i])
				if maxCmp < nowCmp maxCmp = nowCmp					
			}
		}
		return maxCmp
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ fun) -> BoxFunc^
	{
		pars := ref itBox.itPars

		asL := pars[0].first->{TypeFuncLambda^}
		asB := ((pars[0].first.Base)->{TypeFunc^})

		IsCompl := false

		if not asB.RetRef {
			IsCompl = IsComplexType(asB.RetType)	
		}

		ToSet := "%Wut## = bitcast " + pars[0].first.GetName() + " #1 to i8*\n"
		ToSet = ToSet + "%Func## = load " +pars[0].first.Base.GetName() + "* , " + pars[0].first.GetName() + " #1\n"


		if IsCompl or asB.RetType == GTypeVoid{
			ToSet = ToSet + "call "
		}else{
			ToSet = ToSet + "#0 = call " 
		}
		ToSet = ToSet + pars[0].first.Base.GetName() + "%Func##(i8*  %Wut##"

		//if Pars.Size() != 0 or IsCompl ToSet = ToSet + " , "
		if IsCompl
		{
			ToSet = ToSet + " , " + asB.RetType.GetName() + "* #0"
		}

		for i : pars.Size()
		{
			if i != 0{
			ToSet = ToSet + " , "

				ToSet = ToSet + asB.Pars[i].GetName()
				if asB.ParsIsRef[i]{
					ToSet = ToSet + "*"
				}
				ToSet = ToSet + " #" + (i + 1)
			}
		}
		ToSet = ToSet + ")\n"
		
		return new BuiltInFuncMega("()",fun,ToSet)
	}
}
BuiltInTemplateUnroll := class extend BoxTemplate
{
	ToClass := BoxClass^

	this := !(BoxClass^ toAdd) -> void
	{
		FuncName = "."
		OutputName = "error"

		ToClass = toAdd

		emptType := Queue.{Type^}()
		emptType.Push(toAdd.ClassType)
		miniArr := true
		MyFuncType = GetFuncType(emptType,miniArr&,null->{Type^},false,false)
		IsMethod = true
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts

		if pars.Size() != 1 return 255
		if not pars[0].first is TypeClass return 255
		if consts.Size() != 1 return 255
		if not consts[0] is ObjStr return 255

		asStrT := (consts[0]->{ObjStr^})
		asStr := asStrT.GetString()
		
		for ToClass.Params.Size()
		{
			if ToClass.Params[it].ItName == asStr
				return 0
			
		}
		for ToClass.FakeParams.Size()
		{
			if ToClass.FakeParams[it].ItName == asStr
				return 0
		}
		return 255
	}

	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ fun) -> BoxFunc^
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts
		
		Name := string
		if consts.Size() != 1 return null
		if not consts[0] is ObjStr
			return null
		AsStrObj := (consts[0]->{ObjStr^})
		Name = (AsStrObj.GetString())

		pos := -1
		for ToClass.Params.Size()
		{
			if ToClass.Params[it].ItName == Name
			{
				pos = it
			}
		}
		if pos == -1 
		{
			for ToClass.FakeParams.Size()
			{
				if ToClass.FakeParams[it].ItName == Name
				{
					pos = it
				}
			}
			if pos != -1
			{
				CType := ((ToClass.ClassType)->{Type^})
				CTypeP := CType.GetPoint()

				FP := ToClass.FakeParams[pos]
				if FP.Atter.GetValue() != "~ind" return null //TODO: check in GetPrior and add stuff like 'array[2]'
				FPN := ((FP.Atter)->{ObjIndent^}).MyStr
				pos2 := -1
				midType := FieldParam^

				for ToClass.Params.Size()
				{
					if ToClass.Params[it].ItName == FPN {
						midType = ToClass.Params[it]
						pos2 = it
					}
				}
				if pos2 == -1 return null //TODO: check in GetPrior

				if ToClass.ContainVirtual pos2 += 1
				return  new BuiltInFuncUno(".",CType,true,FP.ResultType,true,
				"%Pre## = getelementptr "sbt + (CType.GetName()) + " , " + (CTypeP.GetName()) + " #1, i32 0, i32 "+pos2+"\n" +
				"#0 = bitcast " + midType.ResultType.GetName() + "* %Pre## to " + FP.ResultType.GetName() + "*\n")
			}
			ErrorLog.Push("Cannot find field "+Name+"\n")
			return null
		}

		CType := ((ToClass.ClassType)->{Type^})
		CTypeP := CType.GetPoint()

		usePos := pos
		if ToClass != null
			if ToClass.ContainVirtual {
				usePos = pos + 1
			}
		
	return new BuiltInFuncUno(".",CType,true,ToClass.Params[pos].ResultType,true,
		"#0 = getelementptr "sbt + (CType.GetName()) + " , " + (CTypeP.GetName()) + " #1, i32 0, i32 "+usePos+"\n")
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
BuiltInLenArr := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "![]"
		OutputName = "error"

		//emptType := Queue.{Type^}()
		//emptType.Push(GetType("int"))
		//emptType.Push(GetType("int"))
		//MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts

		if consts.Size() != 0 return 255
		if pars.Size() == 0 return 255
		for pars
			if not it.first is TypeStandart
				return 255
		return 0
	}
	CreateFuncPointer := virtual !(FuncInputBox itBox) -> TypeFunc^
	{
		pars := ref itBox.itPars
		newQueue := Queue.{Type^}()
		for pars.Size() newQueue.Push(pars[0].first)
		return GetFuncType(newQueue,null->{bool^},pars[0].first.GetArray(pars.Size()),false,false)
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ fun) -> BoxFunc^
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts
		toCreate := ""

		baseType := pars[0].first
		retType := baseType.GetArray(pars.Size())

		baseTypeN := baseType.GetName()

		asBase := retType->{Type^}
		retTypeN := asBase.GetName()

		for i : pars.Size()
		{
			toCreate = toCreate + "%TPre##n" + i + " = getelementptr " + retTypeN + " , " + retTypeN + "* #0,i32 0,i32 " + i + "\n"
			toCreate = toCreate + "store " + baseTypeN + " #" + (i + 1) + " , " + baseTypeN + "* %TPre##n" + i + "\n"
		}

		//newQueue := Queue.{Type^}()
		//for pars.Size() newQueue.Push(pars[0].first)

		itFunc := new BuiltInFunc
		itFunc.MyFuncType = fun //GetFuncType(newQueue,null->{bool^},retType,false,false)
		itFunc.ToExe = toCreate
		itFunc.IsRetComplex = true

		return itFunc
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
BuiltInTemplateNew := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "new"
		OutputName = "error"

		emptType := Queue.{Type^}()
		emptType.Push(GTypeInt)
		emptType.Push(GTypeInt)
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts

		if pars.Size() != 2 return 255
		if not (pars[0].first is TypeStandart) return 255
		if not (pars[1].first is TypeStandart) return 255
		if consts.Size() != 1 return 255
		if not (consts[0] is ObjType) return 255
		return 0
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ fun) -> BoxFunc^
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts

		ResType := Type^
		if consts.Size() != 1 return null
		if consts[0].GetValue() != "~type" 
			return null
		AsTypeObj := (consts[0]->{ObjType^})
		ResType = AsTypeObj.MyType
		ResP := ResType.GetFatArray()
		ExtraSize := ResType.GetAlign()
		if ExtraSize < 4 ExtraSize = 4

		return new BuiltInFuncBinar("new",pars[0].first,false,pars[1].first,false,ResP,
			"%PreTotalSize## = mul i32 #1,#2"sbt + "\n" +
			"%TotalSize## = add i32 %PreTotalSize##, " + ExtraSize + "\n" +
			"%PrePtr## = call i8*(i32)@malloc(i32 %TotalSize##)\n"+
			"%PtrInt## = ptrtoint i8* %PrePtr## to i64\n" + 
			"%RealPtrInt## = add i64 %PtrInt## , " + ExtraSize + "\n" +
			"%SizePtrInt## = sub i64 %RealPtrInt## , 4 \n" +
			"%SizePtr## = inttoptr i64 %SizePtrInt## to i32*\n" + 
			"store i32 #2, i32* %SizePtr##\n" +
			"#0 = inttoptr i64 %RealPtrInt## to " + ResP.GetName() + "\n")
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
BuiltInTemplateNext := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "->"
		OutputName = "error"

		emptType := Queue.{Type^}()
		emptType.Push(null->{Type^})
		MyFuncType = GetFuncType(emptType,null->{bool^},GTypeInt,false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts

		if consts.Size() != 1 return 255
		if not (consts[0] is ObjStr) return 255
		if pars.Size() != 1 return 255

		asObjStr := consts[0]->{ObjStr^}
		asStr := asObjStr.GetString()
		
		if asStr == "len"
		{
			if not (pars[0].first is TypeFatArr or pars[0].first is TypeArr) return 255
			return 0
		}
		if asStr == "begin"
		{
			if pars[0].first == TypeTable[13] or pars[0].first == TypeTable[14] return 0
			return 255
		}
		if asStr == "end"
		{
			if pars[0].first == TypeTable[13] or pars[0].first == TypeTable[14] return 0
			return 255
		}

		return 255
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ fun) -> BoxFunc^
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts

		asPreStr := consts[0]->{ObjStr^}
		asStr := asPreStr.GetString()

		if asStr == "begin"
		{
			return new BuiltInFuncUno("->",pars[0].first,false,pars[0].first.Base,false,
					"#0 = extractvalue "sbt + pars[0].first.GetName() + " #1,0\n")
		}else
		if asStr == "end"
		{
			return new BuiltInFuncUno("->",pars[0].first,false,pars[0].first.Base,false,
					"#0 = extractvalue "sbt + pars[0].first.GetName() + " #1,1\n")
		}else
		if asStr == "len"
		{
			if pars[0].first is TypeArr
			{	
				asType := (pars[0].first)->{TypeArr^}
				pre := new BuiltInFuncUno("->",pars[0].first,true,GTypeInt,false,
					"#0 = add i32 0," + asType.Size + "\n")
				pre.IsSelfPre = true
				return pre
			}else{
				return new BuiltInFuncUno("->",pars[0].first,false,GTypeInt,false,
				"%PreP## = bitcast "sbt + pars[0].first.GetName() + " #1 to i32*\n" + 
				"%PreI## = getelementptr i32, i32* %PreP##,i32 -1\n" +
				"#0 = load i32 , i32 * %PreI##\n")
			}
		}
		return null
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
BuiltInTemplateCmpPoints := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "=="
		OutputName = "error"

		emptType := Queue.{Type^}()
		emptType.Push(null->{Type^})
		emptType.Push(null->{Type^})
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts

		if consts.Size() != 0 return 255
		if pars.Size() != 2 return 255
		if not (pars[0].first is TypePoint or pars[0].first is TypeFatArr) return 255
		if not (pars[1].first is TypePoint or pars[1].first is TypeFatArr) return 255
		return 0
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ fun) -> BoxFunc^
	{
		pars := ref itBox.itPars
		return new BuiltInFuncBinar("->",pars[0].first,false,pars[1].first,false,GTypeBool,false,
			"%PreOne## = bitcast "sbt + pars[0].first.GetName() + " #1 to i8*\n" +
			"%PreTwo## = bitcast "    + pars[1].first.GetName() + " #2 to i8*\n" +
			"#0 = icmp eq i8* %PreOne##,%PreTwo##\n")
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
BuiltInTemplateCmpPointsNE := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "!="
		OutputName = "error"

		emptType := Queue.{Type^}()
		emptType.Push(null->{Type^})
		emptType.Push(null->{Type^})
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts

		if consts.Size() != 0 return 255
		if pars.Size() != 2 return 255
		if not (pars[0].first is TypePoint or pars[0].first is TypeFatArr) return 255
		if not (pars[1].first is TypePoint or pars[1].first is TypeFatArr) return 255
		return 0
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ fun) -> BoxFunc^
	{
		pars := ref itBox.itPars
		return new BuiltInFuncBinar("->",pars[0].first,false,pars[1].first,false,GTypeBool,false,
			"%PreOne## = bitcast "sbt + pars[0].first.GetName() + " #1 to i8*\n" +
			"%PreTwo## = bitcast "    + pars[1].first.GetName() + " #2 to i8*\n" +
			"#0 = icmp ne i8* %PreOne##,%PreTwo##\n")
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
BuiltInTemplateRefEx := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "."
		OutputName = "error"

		emptType := Queue.{Type^}()
		emptType.Push(null->{Type^})
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
		IsMethod = true
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		if itBox.itPars.Size() != 1 return 255
		return 0
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ fun) -> BoxFunc^
	{
		pars := ref itBox.itPars
		consts := ref itBox.itConsts
		asType := consts[0]->{ObjType^}
		return new BuiltInFuncUno("->",pars[0].first,true,asType.MyType,true,
			"#0 = bitcast " + pars[0].first.GetPoint().GetName() + " #1 to " + asType.MyType.GetPoint().GetName() + "\n")
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}

AddTemplates := !() -> void
{
	BuiltInTemplates.Push(new BuiltInTemplatePointArr())
	BuiltInTemplates.Push(new BuiltInTemplateSet())
	BuiltInTemplates.Push(new BuiltInTemplateNext())
	//BuiltInTemplates.Push(new BuiltInTemplateStorePoint())
	BuiltInTemplates.Push(new BuiltInTemplateGetRef())

	BuiltInTemplates.Push(new BuiltInTemplateTypeInfo())

	//GlobalUnroll = new BuiltInTemplateUnroll()
	//GlobalNew = new BuiltInTemplateNew()
	GlobalUnpoint = new BuiltInTemplatePoint()
	GlobalRefExc = new BuiltInTemplateRefEx()
	GlobalExcArr = new BuiltInTemplateExcArr()

	//BuiltInTemplates.Push(new BuiltInTemplateExcFatArr())
	BuiltInTemplates.Push(GlobalUnpoint)
	BuiltInTemplates.Push(new BuiltInLenArr())
	BuiltInTemplates.Push(new BuiltInTemplateCmpPoints())
	BuiltInTemplates.Push(new BuiltInTemplateCmpPointsNE())
	BuiltInTemplates.Push(new BuiltInTemplateCheckPoint())
	BuiltInTemplates.Push(new BuiltInTemplateVec4fGet())

	BuiltInTemplates.Push(new BuiltInPointAdd())
	BuiltInTemplates.Push(new BuiltInPointSub())

	BuiltInTemplates.Push(new BuiltInLambdaCall())

	//BuiltInTemplates.Push(GlobalUnroll)
}

CreateBuiltIns := !() -> void
{
	AddTemplates()

	BoolT := GetType("bool")
	VoidT := GetType("void")
	VoidP := VoidT.GetPoint()
	DoubleT := GetType("double")

	preRes := int[4]
	preRes[0] = 8
	preRes[1] = 16
	preRes[2] = 32
	preRes[3] = 64
	preRes2 := string[2]
	preRes2[0] = "s"
	preRes2[1] = "u"

	for iPre : 4 for jPre : 4
	for IsS1Pre : 2
	for IsS2Pre : 2
	{
		i := preRes[iPre]
		j := preRes[jPre]
		IsS1 := preRes2[IsS1Pre]
		IsS2 := preRes2[IsS2Pre]
		from := GetType(IsS1 + i)
		to := GetType(IsS2 + j)
		if i > j
		{
			BuiltInFuncs.Push(new BuiltInFuncUno("->{}",from,false,to,false,"#0 = trunc " + from.GetName() + " #1 to " + to.GetName() + "\n"))
		}
		if i < j
		{
			if IsS1 == "s"
			{
				BuiltInFuncs.Push(new BuiltInFuncUno("->{}",from,false,to,"#0 = sext " + from.GetName() + " #1 to " + to.GetName() + "\n"))
			}else{
				BuiltInFuncs.Push(new BuiltInFuncUno("->{}",from,false,to,"#0 = zext " + from.GetName() + " #1 to " + to.GetName() + "\n"))
			}
		}
		//if i == j
		//{
		//}
	}

	for itPre : 4
	{
		it := preRes[itPre]
		for IsSPre : 2
		{
			IsS := preRes2[IsSPre]
			PType := GetType(IsS + it) // u8 s32 ...
			BuiltInFuncs.Push(new BuiltInFuncBinar("=",PType,true,PType,false,PType,"store i" + it + " #2, i" + it + "* #1\n"
												+"#0 = add i" + it + " #2,0\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("+",PType,false,PType,false,PType,"#0 = add i" + it + " #1,#2\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("-",PType,false,PType,false,PType,"#0 = sub i" + it + " #1,#2\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("*",PType,false,PType,false,PType,"#0 = mul i" + it + " #1,#2\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("div",PType,false,PType,false,PType,"#0 = " + IsS + "div  i" + it + " #1,#2\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("%",PType,false,PType,false,PType,"#0 = " + IsS + "rem  i" + it + " #1,#2\n"))
			
			BuiltInFuncs.Push(new BuiltInFuncUno(". -",PType,false,PType,false,"#0 = sub i" + it + " 0,#1\n"))
			
			BuiltInFuncs.Push(new BuiltInFuncBinar("/",PType,false,PType,false,GetType("float"),
										"%Pre1## = " + IsS + "itofp i" + it + " #1 to float\n" +
										"%Pre2## = " + IsS + "itofp i" + it +" #2 to float\n" +
										"#0 = fdiv float %Pre1##,%Pre2##\n"))

			BuiltInFuncs.Push(new BuiltInFuncBinar("==",PType,false,PType,false,BoolT,"#0 = icmp eq i" + it + " #1,#2\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("!=",PType,false,PType,false,BoolT,"#0 = icmp ne i" + it + " #1,#2\n"))

			BuiltInFuncs.Push(new BuiltInFuncBinar("and_b",PType,false,PType,false,PType,"#0 = and i" + it + " #1,#2\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("or_b",PType,false,PType,false,PType,"#0 = or i" + it + " #1,#2\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("xor_b",PType,false,PType,false,PType,"#0 = xor i" + it + " #1,#2\n"))
			BuiltInFuncs.Push(new BuiltInFuncUno(". not_b",PType,false,PType,"#0 = xor i" + it + " #1,-1\n"))

			BuiltInFuncs.Push(new BuiltInFuncBinar(">=",PType,false,PType,false,BoolT,"#0 = icmp "+ IsS +"ge i" + it + " #1,#2\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("<=",PType,false,PType,false,BoolT,"#0 = icmp "+ IsS +"le i" + it + " #1,#2\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar(">",PType,false,PType,false,BoolT,"#0 = icmp "+ IsS +"gt i" + it + " #1,#2\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("<",PType,false,PType,false,BoolT,"#0 = icmp "+ IsS +"lt i" + it + " #1,#2\n"))


			BuiltInFuncs.Push(new BuiltInFuncUno("->{}",PType,false,BoolT,"#0 = icmp ne i"+it+" #1 ,0\n"))
			BuiltInFuncs.Push(new BuiltInFuncUno("->{}",PType,false,DoubleT,"#0 = "+IsS+"itofp i"+it+" #1 to double\n"))

			BuiltInFuncs.Push(new BuiltInFuncBinar("+=",PType,true,PType,false,PType,"#0pre = load i" + it + " , i" + it + "* #1\n"
												+"#0 = add i" + it + " #2,#0pre\n"
												+"store i"+it+" #0, i"+it+"* #1\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("-=",PType,true,PType,false,PType,"#0pre = load i" + it + " , i" + it + "* #1\n"
												+"#0 = sub i" + it + " #0pre,#2\n"
												+"store i"+it+" #0, i"+it+"* #1\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("*=",PType,true,PType,false,PType,"#0pre = load i" + it + " , i" + it + "* #1\n"
												+"#0 = mul i" + it + " #2,#0pre\n"
												+"store i"+it+" #0, i"+it+"* #1\n"))

			BuiltInFuncs.Push(new BuiltInFuncUno("++",PType,true,PType,"#0 = load i" + it + " , i" + it + "* #1\n"
												+"#0Pre = add i" + it + " 1,#0\n"
												+"store i"+it+" #0Pre, i"+it+"* #1\n"))
			BuiltInFuncs.Push(new BuiltInFuncUno("--",PType,true,PType,"#0 = load i" + it + " , i" + it + "* #1\n"
												+"#0Pre = sub i" + it + " #0,1\n"
												+"store i"+it+" #0Pre, i"+it+"* #1\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("<<",PType,false,PType,false,PType, "#0 = shl i" + it + " #1,#2\n"))
		}
		BuiltInFuncs.Push( new BuiltInFuncUno("->{}", GetType("s" + it), false,GetType("u" + it), "#0 = add i" + it + " #1,0"))
	}
	BuiltInFuncs.Push(new BuiltInFuncBinar("in",GetType("int"),false,TypeTable[13],false,BoolT,
											"br label %Start##\n"sbt +
											"Start##:" +
											"%First## = extractvalue %RangeTypeInt #2,0\n"+
											"%T1T## = icmp sge i32 #1,%First##\n" + 
											"br i1 %T1T## ,label %OnNext##, label %OnEnd##\n" +
											"OnNext##:\n"+
											"%Second## = extractvalue %RangeTypeInt #2,1\n" +
											"%T2T## = icmp sle i32 #1, %Second##\n" +
											"br label %OnEnd##\n" +
											"OnEnd##:\n" +
											"#0 = phi i1 [0, %Start##], [%T2T##, %OnNext##]\n"))
	
	preRes3 := string[2]
	preRes3[0] = "float"
	preRes3[1] = "double"
	for itPre : 2 // half?
	{
		it := preRes3[itPre]
		PType := GetType(it)
		BuiltInFuncs.Push(new BuiltInFuncBinar("=",PType,true,PType,false,PType,"store " + it + " #2, " + it + "* #1\n"
											+"#0 = fadd " + it + " #2,0.0\n"))
		BuiltInFuncs.Push(new BuiltInFuncBinar("+",PType,false,PType,false,PType,"#0 = fadd " + it + " #1,#2\n"))
		BuiltInFuncs.Push(new BuiltInFuncBinar("-",PType,false,PType,false,PType,"#0 = fsub " + it + " #1,#2\n"))
		BuiltInFuncs.Push(new BuiltInFuncBinar("*",PType,false,PType,false,PType,"#0 = fmul " + it + " #1,#2\n"))
		BuiltInFuncs.Push(new BuiltInFuncBinar("/",PType,false,PType,false,PType,"#0 = fdiv " + it + " #1,#2\n"))
		BuiltInFuncs.Push(new BuiltInFuncUno(". -",PType,false,PType,false,"#0 = fsub " + it + " 0.0,#1\n"))

		BuiltInFuncs.Push(new BuiltInFuncBinar("+=",PType,true,PType,false,PType,"#0pre = load " + it +" , "+ it +"* #1\n"
											+"#0 = fadd " + it + " #2,#0pre\n"
											+"store "+it+" #0, "+it+"* #1\n"))
		BuiltInFuncs.Push(new BuiltInFuncBinar("-=",PType,true,PType,false,PType,"#0pre = load " + it + " , " + it + "* #1\n"
											+"#0 = fsub " + it + " #0pre,#2\n"
											+"store "+it+" #0, "+it+"* #1\n"))
		BuiltInFuncs.Push(new BuiltInFuncBinar("*=",PType,true,PType,false,PType,"#0pre = load " + it + " , " + it + "* #1\n"
											+"#0 = fmul " + it + " #2,#0pre\n"
											+"store "+it+" #0, "+it+"* #1\n"))
		BuiltInFuncs.Push(new BuiltInFuncBinar("/=",PType,true,PType,false,PType,"#0pre = load " + it + " , " + it + "* #1\n"
											+"#0 = fdiv " + it + " #0pre,#2\n"
											+"store "+it+" #0, "+it+"* #1\n"))


		BuiltInFuncs.Push(new BuiltInFuncBinar(">=",PType,false,PType,false,BoolT,"#0 = fcmp uge " + it + " #1,#2\n"))
		BuiltInFuncs.Push(new BuiltInFuncBinar("<=",PType,false,PType,false,BoolT,"#0 = fcmp ule " + it + " #1,#2\n"))
		BuiltInFuncs.Push(new BuiltInFuncBinar(">",PType,false,PType,false,BoolT,"#0 = fcmp ugt " + it + " #1,#2\n"))
		BuiltInFuncs.Push(new BuiltInFuncBinar("<",PType,false,PType,false,BoolT,"#0 = fcmp ult " + it + " #1,#2\n"))
		if it == "float"
		{
			BuiltInFuncs.Push(new BuiltInFuncBinar("**",PType,false,PType,false,PType,"#0 = call float @llvm.pow.f32(float #1,float #2)\n"))
		}else{
			BuiltInFuncs.Push(new BuiltInFuncBinar("**",PType,false,PType,false,PType,"#0 = call double @llvm.pow.f64(double #1,double #2)\n"))
		}
	}

	BuiltInFuncs.Push(new BuiltInFuncBinar("=",BoolT,true,BoolT,false,BoolT,"store i1 #2, i1* #1\n"
										+"#0 = add i1 #2,0\n"))
	BuiltInFuncs.Push(new BuiltInFuncBinar("==",BoolT,false,BoolT,false,BoolT,"#0 = icmp eq i1 #1,#2\n"))
	BuiltInFuncs.Push(new BuiltInFuncBinar("!=",BoolT,false,BoolT,false,BoolT,"#0 = icmp ne i1 #1,#2\n"))

	
	BuiltInFuncs.Push(new BuiltInFuncUno("->{}",GetType("double"),false,GetType("float"),"#0 = fptrunc double #1 to float\n"))
	BuiltInFuncs.Push(new BuiltInFuncUno("->{}",GetType("float"),false,GetType("double"),"#0 = fpext float #1 to double\n"))

	BuiltInFuncs.Push(new BuiltInFuncUno("->{}",GetType("int"),false,GetType("float"),"#0 = sitofp i32 #1 to float\n"))


	BuiltInFuncs.Push(new BuiltInFuncUno(". not",BoolT,false,BoolT,"#0 = xor i1 #1,1\n"))
//	BuiltInFuncs.Push(new BuiltInFuncBinar("or",BoolT,false,BoolT,false,BoolT,	"#^1" +
//											"br i1 #1, label %End##, label %Next##\n" +
//											"Next##:\n" +
//											"#^2" +
//											"br label %End##\n" +
//											"End##:\n" +
//											"#0 = select i1 #1, i1 true, i1 #2\n"))
//	BuiltInFuncs.Push(new BuiltInFuncBinar("and",BoolT,false,BoolT,false,BoolT,	"#^1" +
//											"br i1 #1, label %Next##, label %End##\n" +
//											"Next##:\n" +
//											"#^2" +
//											"br label %End##\n" +
//											"End##:\n" +
//											"#0 = select i1 #1, i1 #2, i1 false\n"))
	BuiltInFuncs.Push(new BuiltInFuncBinar("or",BoolT,false,BoolT,false,BoolT,
											"br label %First2Part##\n" +
											"First2Part##:\n" +
											"#^1" +
											"br label %FirstPart##\n" +
											"FirstPart##:\n" +
											"br i1 #1, label %End##, label %Next##\n" +
											"Next##:\n" +
											"#^2" +
											"br label %PostNext##\n" +
											"PostNext##:\n" +
											"br label %End##\n" +
											"End##:\n" +
											"#0 = phi i1 [true,%FirstPart##] ,[#2,%PostNext##]\n"))
	BuiltInFuncs.Push(new BuiltInFuncBinar("and",BoolT,false,BoolT,false,BoolT,	
											"br label %First2Part##\n" +
											"First2Part##:\n" +
											"#^1" + 
											"br label %FirstPart##\n" +
											"FirstPart##:\n" +
											"br i1 #1, label %Next##, label %End##\n" +
											"Next##:\n" +
											"#^2" +
											"br label %PostNext##\n" +
											"PostNext##:\n" +
											"br label %End##\n" +
											"End##:\n" +
											"#0 = phi i1 [false,%FirstPart##] , [#2,%PostNext##]\n"))
	BuiltInFuncs.Push( new BuiltInSuffix("f",GTypeDouble,false,GTypeFloat,"#0 = fptrunc double #1 to float\n"))
	BuiltInFuncs.Push( new BuiltInSuffix("L",GetType("int"),false,GetType("s64"),"#0 = sext i32 #1 to i64\n"))
	BuiltInFuncs.Push( new BuiltInSuffix("pi",GTypeFloat,false,GTypeFloat,"%Pre## = fptrunc double 3.14159265389 to float\n" +
					"#0 = fmul float #1,%Pre##\n"))
	BuiltInFuncs.Push( new BuiltInSuffix("deg",GetType("float"),false,GetType("float"),"%Pre## = fptrunc double 0.017453292521161111 to float\n" +
					"#0 = fmul float #1,%Pre##\n"))
	BuiltInFuncs.Push( new BuiltInSuffix("deg",GetType("int"),false,GetType("float"),"%Pre## = fptrunc double 0.017453292521161111 to float\n" +
					"%PrePre## = sitofp i32 #1 to float\n" +
					"#0 = fmul float %PrePre##,%Pre##\n"))
	BuiltInFuncs.Push( new BuiltInSuffix("pi",GetType("int"),false,GetType("float"),
					"%PrePre## = sitofp i32 #1 to float\n" +
					"%Pre## = fptrunc double 3.14159265389 to float\n" +
					"#0 = fmul float %PrePre##,%Pre##\n"))
	RangeFuncs()
	Vec4fFuncs()
}

RangeFuncs := !() -> void
{
	intType := GetType("int")
	rangeType := TypeTable[13]
	BuiltInFuncs.Push( new BuiltInFuncBinar("..",intType,false,intType,false,rangeType,"%PreR## = insertvalue %RangeTypeInt undef,i32 #1, 0\n" 
												+"#0 = insertvalue %RangeTypeInt %PreR##,i32 #2,1\n"))

	BuiltInFuncs.Push( new BuiltInFuncBinar("=",rangeType,true,rangeType,false,rangeType,"store %RangeTypeInt #2, %RangeTypeInt* #1\n"))

}

Vec4fFuncs := !() -> void
{
	F4T := GetType("vec4f")
	FT := GetType("float")
	F4N := F4T.GetName()



	Typs := Type^[2]
	Typs[0] = F4T
	Typs[1] = GetType("quantf")
	
	for NTPre: Typs->len
	{
		NT := Typs[NTPre]
		BuiltInFuncs.Push( new BuiltInFuncBinar("+",NT,false,NT,false,NT,"#0 = fadd " + F4N + " #1 , #2\n"))
		BuiltInFuncs.Push( new BuiltInFuncBinar("-",NT,false,NT,false,NT,"#0 = fsub " + F4N + " #1 , #2\n"))
		BuiltInFuncs.Push( new BuiltInFuncBinar("/",NT,false,NT,false,NT,"#0 = fdiv " + F4N + " #1 , #2\n"))
		BuiltInFuncs.Push( new BuiltInFuncBinar("*",NT,false,NT,false,NT,"#0 = fmul " + F4N + " #1 , #2\n"))
		BuiltInFuncs.Push( new BuiltInFuncBinar("=",NT,true,NT,false,GTypeVoid,"store " + F4N + " #2 ," + F4N + "* #1\n"))
	
		BuiltInFuncs.Push(new BuiltInFuncBinar("+=",NT,true,NT,false,NT,"#0pre = load " + F4N +" , "+ F4N +"* #1\n"
											+"#0 = fadd " + F4N + " #2,#0pre\n"
											+"store "+ F4N +" #0, "+F4N+"* #1\n"))
		BuiltInFuncs.Push(new BuiltInFuncBinar("-=",NT,true,NT,false,NT,"#0pre = load " + F4N +" , "+ F4N +"* #1\n"
											+"#0 = fsub " + F4N + " #0pre,#2\n"
											+"store "+ F4N +" #0, "+F4N+"* #1\n"))
	
		BuiltInFuncs.Push( new BuiltInFuncBinar("<+>",NT,false,NT,false,FT,"%Pre## = fmul " + F4N + " #1 , #2\n" + 
			"#0 = call fast float @llvm.experimental.vector.reduce.fadd.f32.v4f32(float undef,<4 x float> %Pre##)\n"))

		BuiltInFuncs.Push( new BuiltInFuncTypeTimes(". this",FT,1,NT,"#0 = insertelement " + F4N + " undef, float #1,i32 0\n"))
		BuiltInFuncs.Push( new BuiltInFuncTypeTimes(". this",FT,2,NT,"%Pre3p## = insertelement " + F4N + " undef, float #1,i32 0\n" + 
									"#0 = insertelement " + F4N + " %Pre3p##, float #2,i32 1\n"))
		BuiltInFuncs.Push( new BuiltInFuncTypeTimes(". this",FT,3,NT,"%Pre3p## = insertelement " + F4N + " undef, float #1,i32 0\n" + 
									"%Pre2p## = insertelement " + F4N + " %Pre3p##, float #2,i32 1\n"+
									"#0 = insertelement " + F4N + " %Pre2p##, float #3,i32 2\n"))
		BuiltInFuncs.Push( new BuiltInFuncTypeTimes(". this",FT,4,NT,"%Pre3p## = insertelement " + F4N + " undef, float #1,i32 0\n" + 
									"%Pre2p## = insertelement " + F4N + " %Pre3p##, float #2,i32 1\n"+
									"%Pre1p## = insertelement " + F4N + " %Pre2p##, float #3,i32 2\n"+
									"#0	  = insertelement " + F4N + " %Pre1p##, float #4,i32 3\n"))
	}
}

