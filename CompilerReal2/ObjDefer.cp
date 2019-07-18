deferAddDefer := BoxFunc^ // AddDefer(!()&->void,bool isException)
deferGetDepth := BoxFunc^
deferApply := BoxFunc^


DeferInit2 := !() -> bool
{

	inPars := Queue.{Type^}() ; $temp
	inPars.Push(GTypeVoidP)
	defFunc := GetFuncType(inPars,null,GTypeVoid,false,false) ; $temp

	dummy := new Object ; $temp
	box := new FuncInputBox ; $temp

	depth := FindFunc("internalDeferGetDepth",dummy,box^,false)
	if depth == null return false
	deferGetDepth = depth

	box.itPars.Emplace(defFunc.GetPoint(),false)
	box.itPars.Emplace(GTypeVoidP,false)
	itFunc := FindFunc("internalDeferAdd",dummy,box^,false)
	if itFunc == null return false

	deferAddDefer = itFunc

	box2 := new FuncInputBox ; $temp
	box2.itPars.Emplace(GTypeInt,false)
	applyFunc := FindFunc("internalDeferApply",dummy,box2^,false)
	if applyFunc == null
		return false

	return true
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
