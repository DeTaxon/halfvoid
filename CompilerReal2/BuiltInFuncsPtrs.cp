BuiltInTemplateSet := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "="
		OutputName = "error"

		emptType := Queue.{Type^}() ; $temp
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
					"%TPre## = bitcast "sbt + pars[1].first.GetName() + "* #2 to " + pars[0].first.GetName() + " #d\n" +
					"store " + pars[0].first.GetName() + " %TPre##, " + pars[0].first.GetName() + "* #1 #d\n")
			}else{
			PreRet = new BuiltInFuncBinar("=",pars[0].first,true,pars[1].first,false,GTypeVoid, 
					"%TPre## = bitcast "sbt + pars[1].first.GetName() + " #2 to " + pars[0].first.GetName() + " #d\n" +
					"store " + pars[0].first.GetName() + " %TPre##, " + pars[0].first.GetName() + "* #1 #d\n")
			}
			return PreRet
		}
		ptrA := pars[0].first
		ptrB := pars[1].first
		if ptrA is TypePoint and ptrA.Base is TypeClass and ptrA.Base->{TypeClass^}.ToClass.IsGC
		{
			return new BuiltIn2SetGCPtr("=",funct)
			return new BuiltInFuncBinar("=",pars[0].first,true,pars[1].first,false,GTypeVoid, 
				"%Left## = bitcast "sbt + ptrA.GetName() + "* #1 to i8**\n" + 
				"%Right## = bitcast "sbt + ptrB.GetName() + " #2 to i8*\n" + 
				"call void@" + gcSetValueFunc.OutputName + "(i8** %Left##, i8* %Right##) #d\n"
			)
		}
		if pars[0].first != pars[1].first
		{
			return new BuiltInFuncBinar("=",pars[0].first,true,pars[1].first,false,GTypeVoid, 
					"%TPre## = bitcast "sbt + pars[1].first.GetName() + " #2 to " + pars[0].first.GetName() + " #d\n" +
					"store " + pars[0].first.GetName() + " %TPre##, " + pars[0].first.GetName() + "* #1 #d\n")
		}
		return new BuiltInFuncBinar("=",pars[0].first,true,pars[1].first,false,GTypeVoid, "store "sbt + pars[0].first.GetName() + " #2, " + pars[0].first.GetName() +"* #1 #d\n")
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
BuiltIn2SetGCPtr := class extend BuiltIn2Func
{
	this := !(char^ fName, TypeFunc^ fType) -> void
	{
		FuncName = fName
		MyFuncType = fType
	}
	PrintFunc := virtual !(BuiltIn2Call^ trg,TIOStream f) -> void {
		id := trg.GenId()

		trg.Down.PrintPointPre(f)
		trg.Down.Right.PrintPre(f)
		
		f << "%Left" << id << " = bitcast "
		trg.Down.PrintPointUse(f)
		f << " to i8**\n"
		f << "%Right"<<id<<" = bitcast "
		trg.Down.Right.PrintUse(f) 
		f << " to i8*\n" 
		f << "call void@" << gcSetValueFunc.OutputName << "(i8** %Left"<<id<<", i8* %Right"<<id<<")"
		if DebugMode
		{
			dbgId := CreateDebugCall(trg)
			if dbgId != -1
			{
				f << ", !dbg !" << dbgId
			}
		}
		f << "\n"
	}
}