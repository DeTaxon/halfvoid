BoxBlock := class extend Object
{
	ItFuncs := Queue.{ObjParam^}
	this := !() -> void
	{
	}
	GetValue := virtual !() -> string
	{
		return "{d}"
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Syntax
		{
			SyntaxCompress(this&,PriorityData)
			UnboxParams(this.Down)	
			WorkBag.Push(this&,State_CollectParams)
			
			iter := Down
			while iter != null
			{
				WorkBag.Push(iter,State_Syntax)
				iter = iter.Right
			}
		}
		if pri == State_CollectParams
		{
			iter := Down
			while iter != null
			{
				if iter.GetValue() == "i:=1"
				{
					dynCast := iter->{ObjParam^}
					if dynCast.Down.GetValue() == "~d"
					{
						if dynCast.Down.Down.GetValue() == "!"
						{
							ItFuncs.Push(dynCast)
							PopOutNode(iter)
						}
					}
				}
				iter = iter.Right
			}
		}
	}
}
