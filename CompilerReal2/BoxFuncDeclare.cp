
BoxFuncDeclare := class  extend BoxFunc
{
	this := !(Object^ inPars, Object^ inOutType, string SomeName) -> void
	{
		IsRetRef = false
		OutputName = StrCopy(SomeName)
		FuncName = OutputName
		IsInvalid = not ParseParams(inPars,inOutType,false)

		if IsInvalid inOutType.EmitError("can not parse function\n")
		MethodType = null
	}
	this := !(TypeFunc^ fT, string SomeName) -> void
	{
		FuncName = SomeName
		OutputName = SomeName
		MyFuncType = fT
	}
	PrintGlobal := virtual !(TIOStream f) -> void
	{
		f << "declare "
		if OutCC != null{
			f << " " << OutCC << " "
		}
		MyFuncType.RetType.PrintType(f)
		f << " @" << OutputName
		MyFuncType.PrintSkobs(f)
		f << "\n"
	}

	DoTheWork := virtual !(int pri) -> void
	{
		//empty is ok
	}
}

