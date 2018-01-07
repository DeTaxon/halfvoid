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
		}
		iterForName = iterForName.Up
	}


	if iter.GetValue() == "#declare"
	{
		return new BoxFuncDeclare(ParamsObj,RetT,FName)
	}
	if iter.GetValue() == "{}"
	{
		return null
	}

	return null

}


BoxFunc := class extend Object
{
	MyFuncType := TypeFunc^
	MyFuncParamNames := string^

	ParseParams := !(Object^ root) -> bool
	{
		iter := root
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
	KeepName := virtual !()-> bool
	{
		return true
	}
}

BoxFuncDeclare := class  extend BoxFunc
{
	ItName := string
	this := !(Object^ inPars, Object^ inOutType, string SomeName) -> void
	{
		ItName = SomeName
		IsInvalid = not ParseParams(inPars.Down)

		if not IsInvalid
			MyFuncType.RetType = ParseType(inOutType)

		if MyFuncType.RetType == null IsInvalid = true
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
		PrintGlobalSub(f)
		f << "declare "
		MyFuncType.RetType.PrintType(f)
		f << " @" << ItName
		MyFuncType.PrintSkobs(f)
		f << "\n"
	}
	KeepName := virtual !()-> bool
	{
		return true
	}
}

BoxFuncBody := class extend BoxFunc
{
	this := !(Object^ root) -> void
	{
	}
	KeepName := virtual !()-> bool
	{
		return false
	}
}
