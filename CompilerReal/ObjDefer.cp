ObjDefer := class extend Object
{
	this := !(Object^ dwn) -> void
	{
		Down = dwn
		Down.SetUp(this&)
		WorkBag.Push(this&,State_Start)
	}
	PrintDestructor := virtual !(sfile f) -> void
	{
		Down.PrintInBlock(f)
		Down.PrintDestructor(f)
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
	}
	DoTheWork := virtual !(int pri) ->	void
	{
		WorkBag.Push(Down,pri)
	}
}
