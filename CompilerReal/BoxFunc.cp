ParseFuncDataR := !(Object^ item) -> Object^
{
	iter := item.Down

	IsStatic := false
	IsVirtual := false

	RetT := Object^
	RetT = null

	if iter.GetValue() == "virtual"
	{
		IsVirtual = true
		iter = iter.Right
	}
	if iter.GetValue() == "static"
	{
		IsStatic = true
		iter = iter.Right
	}

	if iter.GetValue() != "!" return null
	iter = iter.Right
	ParamsObj := iter
	iter = iter.Right

	if iter.GetValue() == "->"
	{
		iter = iter.Right
		RetT = iter
		iter = iter.Right
	}

	iterForName := iter.Up
	FName := ""

	while iterForName != null
	{
		if iterForName.GetValue() == "i:=1"
		{
			dynCa := iterForName->{ObjParam^}
			FName = dynCa.MyStr
			iterForName = null
		}else	iterForName = iterForName.Up
	}
	
	if iter.GetValue() == "#declare"
	{
		return new BoxFuncDeclare(ParamsObj,RetT,FName)
	}
	if iter.GetValue() == "{}"
	{
		return new BoxFuncBody(ParamsObj,RetT,FName,iter)
	}

	return null
}


BoxFunc := class extend Object
{
	MyFuncType := TypeFunc^
	MyFuncParamNames := string^

	ParseParams := !(Object^ root) -> bool
	{
		SyntaxCompress(root,PriorityData)
		iter := root.Down
		Pars := Queue.{Object^}()

		Typs := Queue.{Type^}()
		TypsNams := Queue.{string}()
		IsVargsL := false

		Stuff := Queue.{Object^}()

		while iter != null
		{
			Stuff.Push(iter)
			iter = iter.Right
		}

		if Stuff.Size() != 0 Stuff.Push(new ObjSymbol(","))

		while Stuff.NotEmpty()
		{
			if Stuff[0].GetValue() == ","
			{
				if Pars.Size() == 2
				{
					MayType := ParseType(Pars[0])
					MayName := ""

					if MayType == null
					{
						printf("can not parse type\n")
						return false
					}

					if Pars[1].GetValue() == "~ind" 
					{
						MayName = (Pars[1]->{ObjIndent^}).MyStr
					}else{
						printf("only indentificator allowed\n")
						return false
					}
					Typs.Push(MayType)
					TypsNams.Push(MayName.Copy())
					Pars.Clean()		

				}
				if Pars.Size() == 1
				{
					if Pars[0].GetValue() == "..." IsVargsL = true
					Pars.Clean()
				}
				Stuff.Pop()
			}else
			{
				Pars.Push(Stuff.Pop())
			}
		}
		MyFuncType = null
		MyFuncParamNames = null

		if Typs.Size() != 0
		{
			MyFuncParamNames = TypsNams.ToArray()
		}
		MyFuncType = GetFuncType(Typs,IsVargsL)
		return true
	}
	GetValue := virtual !() -> string
	{
		return "!()"
	}
}

BoxFuncDeclare := class  extend BoxFunc
{
	ItName := string
	this := !(Object^ inPars, Object^ inOutType, string SomeName) -> void
	{
		ItName = SomeName
		IsInvalid = not ParseParams(inPars)

		if not IsInvalid
			MyFuncType.RetType = ParseType(inOutType)

		if MyFuncType.RetType == null IsInvalid = true

		if IsInvalid ErrorLog.Push("can not parse function\n")
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
		f << "declare "
		MyFuncType.RetType.PrintType(f)
		f << " @" << ItName
		MyFuncType.PrintSkobs(f)
		f << "\n"
	}
}

BoxFuncBody := class extend BoxFunc
{
	OutputName := string
	TempBody := Object^
	this := !(Object^ inPars, Object^ inOutType, string SomeName, Object^ Stuf) -> void
	{
		if SomeName == "main"
		{
			OutputName = "main"
		}else
		{
			OutputName = new char[50]
			sprintf(OutputName,"func%i",GetNewId())
		}
		IsInvalid = not ParseParams(inPars)

		if not IsInvalid
			MyFuncType.RetType = ParseType(inOutType)

		if MyFuncType.RetType == null IsInvalid = true

		TempBody = Stuf
		if IsInvalid ErrorLog.Push("can not parse function header\n")
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
		PrintGlobalSub(f)
		f << "define "
		MyFuncType.RetType.PrintType(f)
		f << " @" << OutputName
		MyFuncType.PrintSkobs(f)
		f << "\n{\n"

		iter := Down
		while iter != null
		{
			iter.PrintInBlock(f)
			iter = iter.Right
		}

		f << "}\n"
	}
}
