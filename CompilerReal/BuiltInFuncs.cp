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
		while AsmLine[i]
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

		PP := Queue.{Type^}()
		MyFuncType = GetFuncType(PP,null->{bool^},retV,RRetRef,false)

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
	GetPriority := virtual !(Queue.{Type^} pars) -> int
	{
		if pars.Size() != 1 return 255
		return 0
	}
	GetNewFunc := virtual  !(Queue.{Type^} pars,Queue.{Object^} consts, TypeFunc^ funct) -> BoxFunc^
	{
		return new BuiltInFuncUno("&",pars[0],true,pars[0].GetPoint(), 
		"#0 = getelementptr " + pars[0].GetName() + " , " + pars[0].GetName() + "* #1, i32 0\n")
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
	GetPriority := virtual !(Queue.{Type^} pars) -> int
	{
		if pars.Size() != 1 return 255
		if pars[0].GetType() != "point" return 255
		return 0
	}
	GetNewFunc := virtual  !(Queue.{Type^} pars,Queue.{Object^} consts, TypeFunc^ funct) -> BoxFunc^
	{
		return new BuiltInFuncUno("^",pars[0],false,pars[0].Base,true, 
		"#0 = getelementptr " + pars[0].Base.GetName() + " , " + pars[0].GetName() + " #1, i32 0\n")
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
	GetPriority := virtual !(Queue.{Type^} pars) -> int
	{
		if pars.Size() != 2 return 255
		if pars[0].GetType() != "point" return 255
		if pars[1].GetType() != "standart" return 255
		return 0
	}
	GetNewFunc := virtual  !(Queue.{Type^} pars,Queue.{Object^} consts, TypeFunc^ funct) -> BoxFunc^
	{
		if pars[0].GetType() == "arr"
		{
			return new BuiltInFuncBinar("[]",pars[0],true,pars[1],false,pars[0].Base,true,
		"#0 = getelementptr " + pars[0].GetName() + " , " + pars[0].GetName() + "* #1, i32 0, " + pars[1].GetName() + " #2\n")
		}
		return new BuiltInFuncBinar("[]",pars[0],false,pars[1],false,pars[0].Base,true,
		"#0 = getelementptr " + pars[0].Base.GetName() + " , " + pars[0].GetName() + " #1, " + pars[1].GetName() + " #2\n")
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
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
	GetPriority := virtual !(Queue.{Type^} pars) -> int
	{
		if pars.Size() != 1 return 255
		if pars[0].GetType() != "arr" return 255
		return 0
	}
	GetNewFunc := virtual  !(Queue.{Type^} pars,Queue.{Object^} consts, TypeFunc^ funct) -> BoxFunc^
	{
		return new BuiltInFuncUno("->{}",pars[0],true,pars[0].Base.GetPoint(),false,
		"#0 = getelementptr " + pars[0].GetName() + " , " + pars[0].GetName() + "* #1, i32 0,i32 0\n")
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
	GetPriority := virtual !(Queue.{Type^} pars, Queue.{Object^} consts) -> int
	{
		if pars.Size() != 1 return 255
		if pars[0].GetType() != "fatarr" return 255
		//if consts.Size() != 1 return 255
		//if consts[0].GetValue() != "~type" return 255 
		return 0
	}
	GetNewFunc := virtual  !(Queue.{Type^} pars,Queue.{Object^} consts, TypeFunc^ funct) -> BoxFunc^
	{
		//toToT := ((consts[0]) ->{ObjType^}).MyType
		toToT := ((pars[0].Base).GetPoint())
		return new BuiltInFuncUno("->{}",pars[0],false,pars[0].Base.GetPoint(),false,
		"#0 = bitcast " + pars[0].GetName() +  "#1 to " + toToT.GetName() + "\n")
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
//BuiltInTemplateStorePoint := class extend BoxTemplate
//{
//	this := !() -> void
//	{
//		FuncName = "="
//		OutputName = "error"
//
//		emptType := Queue.{Type^}()
//		emptType.Push(null->{Type^})
//		emptType.Push(null->{Type^})
//		arrr := bool[2]
//		arrr[0] = true
//		arrr[1] = false
//		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
//	}
//	GetPriority := virtual !(Queue.{Type^} pars, Queue.{Object^} consts) -> int
//	{
//		if consts.Size() != 0 return 255
//		if pars.Size() != 2 return 255
//		printf("wut %s %s\n",pars[0].GetType(),pars[1].GetType())
//		if pars[0].GetType() != "point" return 255
//
//		if pars[1].GetType() != "point"
//		{
//			if pars[1].GetType() != "fatarr" return 255
//		}
//		return 0
//	}
//	GetNewFunc := virtual  !(Queue.{Type^} pars,Queue.{Object^} consts, TypeFunc^ fun) -> BoxFunc^
//	{
//		return new BuiltInFuncBinar("->{}",pars[0],true,pars[1],false,GetType("void"),
//			"%TPre## = bitcast " + pars[1].GetName() + " #2 to " + pars[0].GetName() + "\n" +
//			"store " + pars[0].GetName() + " %TPre## , " + pars[0].GetPoint().GetName() + " #1\n")
//	}
//	DoTheWork := virtual !(int pri) -> void
//	{
//		
//	}
//}
BuiltInTemplateSet := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "="
		OutputName = "error"

		emptType := Queue.{Type^}()
		emptType.Push(null->{Type^})
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(Queue.{Type^} pars, Queue.{Object^} consts) -> int
	{
		if pars.Size() != 2 return 255
		if pars[0].GetType() != "point" and pars[0].GetType() != "fatarr" return 255
		if pars[1].GetType() != "point" and pars[1].GetType() != "fatarr" return 255
		if pars[1].Base == GetType("void") return 1
		if pars[0].GetType() == "point"
		{
			if pars[0].Base == GetType("void") return 1
			if pars[1].Base == GetType("void") return 1
			if pars[1].Base == pars[0].Base return 0
		}else
		{
			if pars[1].Base == pars[0].Base return 0
		}
		return 255
	}
	GetNewFunc := virtual  !(Queue.{Type^} pars,Queue.{Object^} consts, TypeFunc^ funct) -> BoxFunc^
	{
		if pars[0].GetType() == "point" and pars[0] != pars[1] 
		{
			PreRet := new BuiltInFuncBinar("=",pars[0],true,pars[1],false,GetType("void"), "%TPre## = bitcast " + pars[1].GetName() + " #2 to " + pars[0].GetName() + "\n" +
													"store " + pars[0].GetName() + " %TPre##, " + pars[0].GetName() + "* #1\n")
			return PreRet
		}
		return new BuiltInFuncBinar("=",pars[0],true,pars[0],false,GetType("void"), "store " + pars[0].GetName() + " #2, " + pars[0].GetName() +"* #1\n")
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
	GetPriority := virtual !(Queue.{Type^} pars,Queue.{Object^} consts) -> int
	{
		return ComputePriorFunc(MyFuncType,pars)
	}

	GetNewFunc := virtual  !(Queue.{Type^} pars,Queue.{Object^} consts, TypeFunc^ fun) -> BoxFunc^
	{
		Name := string
		AsStrObj := (consts[0]->{ObjStr^})
		Name = (AsStrObj.GetString())

		CType := ((ToClass.ClassType)->{Type^})
		CTypeP := CType.GetPoint()

		preRetCode := ""

		if ToFunc.MyFuncType.RetType != GetType("void")
			preRetCode = "#0 = "

		preRetCode = preRetCode + ToFunc.MyFuncType.GetName() + "@" + ToFunc.OutputName

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
	GetPriority := virtual !(Queue.{Type^} pars,Queue.{Object^} consts) -> int
	{
		if pars.Size() != 0 return 255
		if consts.Size() != 1 return 255
		if (consts[0].GetValue() != "~str") return 255

		asStrT := (consts[0]->{ObjStr^})
		asStr := asStrT.GetString()
		
		for ToClass.Params.Size()
		{
			if ToClass.Params[it].ItName == asStr
				return 0
			
		}
		return 255
	}

	GetNewFunc := virtual  !(Queue.{Type^} pars,Queue.{Object^} consts, TypeFunc^ fun) -> BoxFunc^
	{
		Name := string
		if consts.Size() != 1 return null
		if consts[0].GetValue() != "~str" 
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
			ErrorLog.Push("Cannot find field "+Name+"\n")
			return null
		}

		CType := ((ToClass.ClassType)->{Type^})
		CTypeP := CType.GetPoint()

		preRet :=  new BuiltInFuncZero(".",ToClass.Params[pos].ResultType,true,
		"#0 = getelementptr " + (CType.GetName()) + " , " + (CTypeP.GetName()) + " %this, i32 0, i32 "+pos+"\n")
		return preRet
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
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
	GetPriority := virtual !(Queue.{Type^} pars,Queue.{Object^} consts) -> int
	{
		if pars.Size() != 1 return 255
		if pars[0].GetType() != "class" return 255
		if consts.Size() != 1 return 255
		if (consts[0].GetValue() != "~str") return 255

		asStrT := (consts[0]->{ObjStr^})
		asStr := asStrT.GetString()
		
		for ToClass.Params.Size()
		{
			if ToClass.Params[it].ItName == asStr
				return 0
			
		}
		return 255
	}

	GetNewFunc := virtual  !(Queue.{Type^} pars,Queue.{Object^} consts, TypeFunc^ fun) -> BoxFunc^
	{
		Name := string
		if consts.Size() != 1 return null
		if consts[0].GetValue() != "~str" 
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
		"#0 = getelementptr " + (CType.GetName()) + " , " + (CTypeP.GetName()) + " #1, i32 0, i32 "+usePos+"\n")
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
	GetPriority := virtual !(Queue.{Type^} pars, Queue.{Object^} consts) -> int
	{
		if consts.Size() != 0 return 255
		if pars.Size() == 0 return 255
		for pars.Size() 
			if pars[it].GetType() != "standart"
				return 255
		return 0
	}
	CreateFuncPointer := virtual !(Queue.{Type^} pars, Queue.{Object^} consts) -> TypeFunc^
	{
		newQueue := Queue.{Type^}()
		for pars.Size() newQueue.Push(pars[0])
		return GetFuncType(newQueue,null->{bool^},pars[0].GetArray(pars.Size()),false,false)
	}
	GetNewFunc := virtual  !(Queue.{Type^} pars,Queue.{Object^} consts, TypeFunc^ fun) -> BoxFunc^
	{
		toCreate := ""

		baseType := pars[0]
		retType := baseType.GetArray(pars.Size())

		baseTypeN := baseType.GetName()
		retTypeN := retType.GetName()

		for i : pars.Size()
		{
			toCreate = toCreate + "%TPre##n" + i + " = getelementptr " + retTypeN + " , " + retTypeN + "* #0,i32 0,i32 " + i + "\n"
			toCreate = toCreate + "store " + baseTypeN + " #" + (i + 1) + " , " + baseTypeN + "* %TPre##n" + i + "\n"
		}

		//newQueue := Queue.{Type^}()
		//for pars.Size() newQueue.Push(pars[0])

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
		emptType.Push(GetType("int"))
		emptType.Push(GetType("int"))
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	//GetPriority := virtual !(Queue.{Type^} pars) -> int
	//{
	//	if pars.Size() != 2 return 255
	//	if pars[0].GetType() != "point" return 255
	//	if pars[1].GetType() != "standart" return 255
	//	return 0
	//}
	GetNewFunc := virtual  !(Queue.{Type^} pars,Queue.{Object^} consts, TypeFunc^ fun) -> BoxFunc^
	{
		ResType := Type^
		if consts.Size() != 1 return null
		if consts[0].GetValue() != "~type" 
			return null
		AsTypeObj := (consts[0]->{ObjType^})
		ResType = AsTypeObj.MyType
		ResP := ResType.GetFatArray()
		ExtraSize := ResType.GetAlign()
		if ExtraSize < 4 ExtraSize = 4

		return new BuiltInFuncBinar("new",pars[0],false,pars[1],false,ResP,
			"%PreTotalSize## = mul i32 #1,#2" + "\n" +
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

		//emptType := Queue.{Type^}()
		//emptType.Push(GetType("int"))
		//emptType.Push(GetType("int"))
		//MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(Queue.{Type^} pars, Queue.{Object^} consts) -> int
	{
		if consts.Size() != 1 return 255
		if consts[0].GetValue() != "~str" return 255

		asObjStr := consts[0]->{ObjStr^}
		asStr := asObjStr.GetString()
		
		if asStr == "len"
		{
			if pars[0].GetType() != "fatarr" return 255
			return 0
		}

		return 255
	}
	GetNewFunc := virtual  !(Queue.{Type^} pars,Queue.{Object^} consts, TypeFunc^ fun) -> BoxFunc^
	{
		asPreStr := consts[0]->{ObjStr^}
		asStr := asPreStr.GetString()

		if asStr == "len"
		{
			return new BuiltInFuncUno("->",pars[0],false,GetType("int"),false,
			"%PreP## = bitcast " + pars[0].GetName() + " #1 to i32*\n" + 
			"%PreI## = getelementptr i32, i32* %PreP##,i32 -1\n" +
			"#0 = load i32 , i32 * %PreI##\n")
		}
		return null
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
	GetPriority := virtual !(Queue.{Type^} pars) -> int
	{
		if pars.Size() != 1 return 255
		return 0
	}
	GetNewFunc := virtual  !(Queue.{Type^} pars,Queue.{Object^} consts, TypeFunc^ fun) -> BoxFunc^
	{
		asType := consts[0]->{ObjType^}
		return new BuiltInFuncUno("->",pars[0],true,asType.MyType,true,
			"#0 = bitcast " + pars[0].GetPoint().GetName() + " #1 to " + asType.MyType.GetPoint().GetName() + "\n")
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

	//GlobalUnroll = new BuiltInTemplateUnroll()
	GlobalNew = new BuiltInTemplateNew()
	GlobalUnpoint = new BuiltInTemplatePoint()
	GlobalRefExc = new BuiltInTemplateRefEx()
	GlobalExcArr = new BuiltInTemplateExcArr()

	BuiltInTemplates.Push(GlobalUnpoint)
	BuiltInTemplates.Push(new BuiltInLenArr())
	//BuiltInTemplates.Push(GlobalUnroll)
}

CreateBuiltIns := !() -> void
{
	AddTemplates()

	BoolT := GetType("bool")
	VoidT := GetType("void")
	VoidP := VoidT.GetPoint()
	DoubleT := GetType("double")

	for ![8,16,32,64]
	{
		for IsS : !["s","u"]
		{
			PType := GetType(IsS + it) // u8 s32 ...
			BuiltInFuncs.Push(new BuiltInFuncBinar("=",PType,true,PType,false,PType,"store i" + it + " #2, i" + it + "* #1\n"
												+"#0 = add i" + it + " #2,0\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("+",PType,false,PType,false,PType,"#0 = add i" + it + " #1,#2\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("-",PType,false,PType,false,PType,"#0 = sub i" + it + " #1,#2\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("*",PType,false,PType,false,PType,"#0 = mul i" + it + " #1,#2\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("div",PType,false,PType,false,PType,"#0 = " + IsS + "div  i" + it + " #1,#2\n"))
			
			BuiltInFuncs.Push(new BuiltInFuncBinar("/",PType,false,PType,false,GetType("float"),
										"%Pre1## = " + IsS + "itofp i" + it + " #1 to float\n" +
										"%Pre2## = " + IsS + "itofp i" + it +" #2 to float\n" +
										"#0 = fdiv float %Pre1##,%Pre2##\n"))

			BuiltInFuncs.Push(new BuiltInFuncBinar("==",PType,false,PType,false,BoolT,"#0 = icmp eq i" + it + " #1,#2\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("!=",PType,false,PType,false,BoolT,"#0 = icmp ne i" + it + " #1,#2\n"))

			BuiltInFuncs.Push(new BuiltInFuncBinar(">=",PType,false,PType,false,BoolT,"#0 = icmp "+ IsS +"ge i" + it + " #1,#2\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("<=",PType,false,PType,false,BoolT,"#0 = icmp "+ IsS +"le i" + it + " #1,#2\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar(">",PType,false,PType,false,BoolT,"#0 = icmp "+ IsS +"gt i" + it + " #1,#2\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("<",PType,false,PType,false,BoolT,"#0 = icmp "+ IsS +"lt i" + it + " #1,#2\n"))


			BuiltInFuncs.Push(new BuiltInFuncUno("->{}",PType,false,BoolT,"#0 = icmp ne i"+it+" #1 ,0\n"))
			BuiltInFuncs.Push(new BuiltInFuncUno("->{}",PType,false,DoubleT,"#0 = "+IsS+"itofp i"+it+" #1 to double\n"))

			BuiltInFuncs.Push(new BuiltInFuncBinar("+=",PType,true,PType,false,PType,"#0pre = load i" + it + " , i" + it + "* #1\n"
												+"#0 = add i" + it + " #2,#0pre\n"
												+"store i"+it+" #0, i"+it+"* #1"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("-=",PType,true,PType,false,PType,"#0pre = load i" + it + " , i" + it + "* #1\n"
												+"#0 = sub i" + it + " #0pre,#2\n"
												+"store i"+it+" #0, i"+it+"* #1\n"))
			BuiltInFuncs.Push(new BuiltInFuncBinar("*=",PType,true,PType,false,PType,"#0pre = load i" + it + " , i" + it + "* #1\n"
												+"#0 = mul i" + it + " #2,#0pre\n"
												+"store i"+it+" #0, i"+it+"* #1\n"))
		}
		BuiltInFuncs.Push(new BuiltInFuncBinar("in",GetType("int"),false,TypeTable[13],false,BoolT,
												"br label %Start##\n" +
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
		BuiltInFuncs.Push( new BuiltInFuncUno("->{}", GetType("s" + it), false,GetType("u" + it), "#0 = add i" + it + " #1,0"))
	}
	
	for !["float","double"] // half?
	{
		PType := GetType(it)
		BuiltInFuncs.Push(new BuiltInFuncBinar("=",PType,true,PType,false,PType,"store " + it + " #2, " + it + "* #1\n"
											+"#0 = fadd " + it + " #2,0.0\n"))
		BuiltInFuncs.Push(new BuiltInFuncBinar("+",PType,false,PType,false,PType,"#0 = fadd " + it + " #1,#2\n"))
		BuiltInFuncs.Push(new BuiltInFuncBinar("-",PType,false,PType,false,PType,"#0 = fsub " + it + " #1,#2\n"))
		BuiltInFuncs.Push(new BuiltInFuncBinar("*",PType,false,PType,false,PType,"#0 = fmul " + it + " #1,#2\n"))
		BuiltInFuncs.Push(new BuiltInFuncBinar("/",PType,false,PType,false,PType,"#0 = fdiv " + it + " #1,#2\n"))

		BuiltInFuncs.Push(new BuiltInFuncBinar(">=",PType,false,PType,false,BoolT,"#0 = fcmp uge " + it + " #1,#2\n"))
		BuiltInFuncs.Push(new BuiltInFuncBinar("<=",PType,false,PType,false,BoolT,"#0 = fcmp ule " + it + " #1,#2\n"))
		BuiltInFuncs.Push(new BuiltInFuncBinar(">",PType,false,PType,false,BoolT,"#0 = fcmp ugt " + it + " #1,#2\n"))
		BuiltInFuncs.Push(new BuiltInFuncBinar("<",PType,false,PType,false,BoolT,"#0 = fcmp ult " + it + " #1,#2\n"))
	}

	BuiltInFuncs.Push(new BuiltInFuncBinar("=",BoolT,true,BoolT,false,BoolT,"store i1 #2, i1* #1\n"
										+"#0 = add i1 #2,0\n"))

	
	BuiltInFuncs.Push(new BuiltInFuncUno("->{}",GetType("double"),false,GetType("float"),"#0 = fptrunc double #1 to float\n"))
	BuiltInFuncs.Push(new BuiltInFuncUno("->{}",GetType("float"),false,GetType("double"),"#0 = fpext float #1 to double\n"))


	BuiltInFuncs.Push(new BuiltInFuncBinar("or",BoolT,false,BoolT,false,BoolT,	"#^1" +
											"br i1 #1, label %End##, label %Next##\n" +
											"Next##:\n" +
											"#^2" +
											"br label %End##\n" +
											"End##:\n" +
											"#0 = select i1 #1, i1 true, i1 #2\n"))
	BuiltInFuncs.Push(new BuiltInFuncBinar("and",BoolT,false,BoolT,false,BoolT,	"#^1" +
											"br i1 #1, label %Next##, label %End##\n" +
											"Next##:\n" +
											"#^2" +
											"br label %End##\n" +
											"End##:\n" +
											"#0 = select i1 #1, i1 #2, i1 false\n"))
	BuiltInFuncs.Push( new BuiltInSuffix("f",GetType("double"),false,GetType("float"),"#0 = fptrunc double #1 to float\n"))
	RangeFuncs()
}

RangeFuncs := !() -> void
{
	intType := GetType("int")
	rangeType := TypeTable[13]
	BuiltInFuncs.Push( new BuiltInFuncBinar("..",intType,false,intType,false,rangeType,"%PreR## = insertvalue %RangeTypeInt undef,i32 #1, 0\n" 
												+"#0 = insertvalue %RangeTypeInt %PreR##,i32 #2,1\n"))

	BuiltInFuncs.Push( new BuiltInFuncBinar("=",rangeType,true,rangeType,false,rangeType,"store %RangeTypeInt #2, %RangeTypeInt* #1\n"))

}
