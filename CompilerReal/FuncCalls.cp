
GetFuncCall := !(Object^ ToParse) -> Object^
{
	iter := ToParse.Down

	if iter == null return null

	if iter.GetValue() == "~ind"
	{
		iter = iter.Right

		if iter.GetValue() == "()"
		{
			Ps := Queue.{Type^}()

			iterT := iter.Down

			while iterT != null
			{
				Ps.Push(iterT.GetType())
				iterT = iterT.Right
			}
			SomeFunc := FindFunc(((iter.Left)->{ObjIndent^}).MyStr,ToParse,Ps)

			if SomeFunc == null ErrorLog.Push("Function not found\n")
			else
			{
				//return NaturalCall		
			}
		}
		
	}else
	{
	}
	return null
}

NaturalCall := class extend ObjResult
{
	this := !(Object^ func, Object^ Pars) -> void 
	{
		Down = Pars
	}
}

//PointCall
//MethodCall
//VirtualMethodCall


