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
		printf("good\n")
		if pri == State_Syntax
		{
			SyntaxCompress(this&,PriorityData)
		}
	}
}
