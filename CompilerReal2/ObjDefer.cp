deferAddDefer := BoxFunc^ // AddDefer(!()&->void,bool isException)
deferPushStack := BoxFunc^ // GetStackDepth()
deferPopStack := BoxFunc^  // PopStack(int Depth)
deferApplyStack := BoxFunc^  // ApplyStack(int Depth)
deferApplyExceptionStack := BoxFunc^  // ApplyExceptionStack(int Depth)

DeferInit := !() -> void
{
	dummy := new Object
	box := new FunctionInputBox ; $temp
	itFunc := FindFunc("internalAddDefer",dummy,box^,false,false)
}

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
