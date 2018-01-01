GetBoxFunc := !(Object^ fromBag) ->Object^
{
	ContainDeclare := false

	iterW := fromBag.Down

	while iterW != null
	{
		if iterW.GetValue() == "#declare" ContainDeclare = true
		iterW = iterW.Right
	}

	if ContainDeclare return new BoxFuncDeclare(fromBag)
	return null
}

BoxFunc := class extend Object
{
	//this := !() 
	MyFuncType := TypeFunc^
	MyFuncParamNames := string^
	PrintGlobal := !(sfile f) -> void
	{
	}

	ParseParams := !(Object^ root) -> bool
	{
		iter := root.Down
		Pars := Queue.{Object^}()

		Typs := Queue.{Type^}()
		TypsNams := Queue.{string}()
		IsVargsL := false

		while iter != null
		{
			if iter.GetValue() == ","
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

				}else
				{
					//TODO: implement
				}
			}else
			{
				Pars.Push(iter)
			}
			iter = iter.Right
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
}

BoxFuncDeclare := class  extend BoxFunc
{
	this := !(Object^ root) -> void
	{
	}
}

BoxFuncBody := class extend BoxFunc
{
	this := !(Object^ root) -> void
	{
	}
	
}
