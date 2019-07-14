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
		if pars[0].first != GetType("vec4f") and pars[0].first != GetType("quantf") return 255
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
		return GetFuncType(outT,retRefArray,newRet,wantRefs,MyFuncType.IsVArgs)
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ funct) -> BoxFunc^
	{
		asNeedPre := itBox.itConsts[0]->{ObjStr^}
		asNeed := asNeedPre.GetString()
		pars := ref itBox.itPars
		
		if StrSize(asNeed) == 1
		{
			x := 0
			if asNeed == "y" x = 1
			if asNeed == "z" x = 2
			if asNeed == "w" x = 3

			if itBox.itPars[0].second
			{
				return new BuiltInFuncUno(".",pars[0].first,true,GTypeFloat,true, "#0 = getelementptr <4 x float> ,<4 x float>* #1, i32 0, i32 "sbt + x + "\n")
			}
			
			return new BuiltInFuncUno(".",pars[0].first,false,GTypeFloat, "#0 = extractelement <4 x float> #1, i32 "sbt + x + "\n")
		}else{
			vecData := int[4]
			for i : 4
			{
				if asNeed[i] == 'x' vecData[i] = 0
				if asNeed[i] == 'y' vecData[i] = 1
				if asNeed[i] == 'z' vecData[i] = 2
				if asNeed[i] == 'w' vecData[i] = 3
			}
			return new BuiltInFuncUno(".",pars[0].first,false,pars[0].first,"#0 = shufflevector <4 x float> #1, <4 x float> undef , <4 x i32> "sbt +
					"<i32 " + vecData[0] + ",i32 " + vecData[1] + ",i32 " + vecData[2] + ",i32 " + vecData[3] + ">\n")
		}
		return null
	}
}
