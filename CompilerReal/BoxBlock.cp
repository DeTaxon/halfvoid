BoxBlock := class extend Object
{
	this := !() -> void
	{
	}
	this := !(Object^ toRepl) -> void
	{
		if toRepl.GetValue() == "{}"
		{	
			Down = toRepl.Down
		}else
		{
			Down = toRepl
		}
		Down.SetUp(this&)
	}
	GetValue := virtual !() -> string
	{
		return "{d}"
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		iter := Down
		while iter != null
		{
			iter.PrintInBlock(f)
			iter = iter.Right
		}
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
