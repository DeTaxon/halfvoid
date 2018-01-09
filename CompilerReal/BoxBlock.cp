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
		if pri == State_Start
		{
			WorkBag.Push(this&,State_Syntax)
		}
		if pri == State_Syntax
		{
			SyntaxCompress(this&,PriorityData)
			UnboxParams(this.Down)	
			
			iter := Down
			while iter != null
			{
				WorkBag.Push(iter,State_Start)
				iter = iter.Right
			}
		}
	}
}
