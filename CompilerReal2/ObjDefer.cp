#import "Tree.cp"
#import "WrappedFunc.cp"

ObjDefer := class extend Object
{
	this := !(Object^ dwn) -> void
	{
		Down = new WrappedFunc(dwn)
		Down.SetUp(this&)
		WorkBag.Push(this&,State_Start)
	}
	PrintDestructor := virtual !(sfile f) -> void
	{
		asWr := Down->{WrappedFunc^}
		asWr.PrintItCall(f)
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
	}
	DoTheWork := virtual !(int pri) -> void
	{
		WorkBag.Push(Down,pri)
	}
}
