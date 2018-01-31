BoxReturn := class extend Object
{
	this := !(Object^ toUse) -> void
	{
		Down = toUse.Down.Right
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			WorkBag.Push(this&,State_Syntax)
			Down.SetUp(this&)
		}
		if pri == State_Syntax
		{
			WorkBag.Push(Down,State_Start)
		}
		if pri == State_GetUse
		{
		}
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		Down.PrintPre(f)
		f << "ret "
		Down.PrintUse(f)
		f << "\n"
	}
}
