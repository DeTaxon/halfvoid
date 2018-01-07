BoxBlock := class extend Object
{
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
	}
}
