BoxFunc := class extend Object
{
	//this := !() 
	MyFuncType := TypeFunc^
	MyFuncParamNames := string^
	PrintGlobal := !(sfile f) -> void
	{
	}

	ParseParams := !(Object^ root) -> void
	{
		iter := root.Down
		Pars := Queue.{Object^}()

		while iter != null
		{
			if iter.GetValue() == ","
			{
				if Pars.Size() == 2
				{
					MayType := ParseType(Pars[0])
					MayName := ""

					if Pars[1].GetValue() == "~ind" 
						MayName = (Pars[1]->{ObjIndent^}).MyStr

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
	}
}

BoxFuncDeclared := class  extend BoxFunc
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
