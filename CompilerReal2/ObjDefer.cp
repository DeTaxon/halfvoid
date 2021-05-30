deferAddDefer2 := BoxFunc^ // AddDefer(!()&->void,bool isException)
deferGetDepth2 := BoxFunc^
deferSkip2 := BoxFunc^


DeferInit2 := !() -> bool
{

	inPars := Queue.{Type^}() ; $temp
	inPars.Push(GTypeVoidP)
	inPars.Push(GTypeBool)
	defFunc := GetFuncType(inPars,null,GTypeVoid,false,false) ; $temp

	dummy := new Object ; $temp
	box := new FuncInputBox ; $temp

	depth := FindFunc("internalDeferGetDepth",dummy,box^,false)
	if depth == null return false
	depth.ParseBlock()
	deferGetDepth2 = depth

	box.itPars.Emplace(defFunc.GetPoint(),false)
	box.itPars.Emplace(GTypeVoidP,false)

	itFunc := FindFunc("internalDeferAdd",dummy,box^,false)
	if itFunc == null return false
	itFunc.ParseBlock()
	deferAddDefer2 = itFunc

	box2 := new FuncInputBox ; $temp
	box2.itPars.Emplace(GTypeInt,false)
	deferSkip2 = FindFunc("internalDeferSkip",dummy,box2^,false)
	if deferSkip2 == null
		return false
	deferSkip2.ParseBlock()


	return true
}
ObjDefer := class extend Object
{
	onExcp := bool
	this := !(Object^ dwn,bool iExp) -> void
	{
		onExcp = iExp
		Down = dwn
		Down.SetUp(this&)
		WorkBag.Push(this&,State_Start)
	}
	PrintDestructor := virtual !(TIOStream f) -> void
	{
		asWr := Down->{WrappedFunc^}
		//asWr.PrintItCall(f)
	}
	PrintInBlock := virtual !(TIOStream f) -> void
	{
		//TODO WORK
		//callAdd := deferAddDefer
		//if onExcp
		//	callAdd = deferAddDeferExcp
		//itr := Up
		//while itr.GetABox() == null
		//{
		//	itr = itr.Up
		//}
		//asWrap := Down->{WrappedFunc^}
		//funcAl := itr.GetABox()
		//if funcAl.ItemBag.IsEmpty()
		//{
		//	f << "call void @" << callAdd.OutputName << "(void(i8*)* @" << asWrap.OutputName << " , i8* null )"
		//}else{
		//	neId := GetNewId()
		//	f << "%T" << neId << " = bitcast " << funcAl.GetAsUse() << " to i8*\n"
		//	f << "call void @" << callAdd.OutputName << "(void(i8*)* @" << asWrap.OutputName << " , i8* %T"<< neId << " )"
		//}
		//if DebugMode
		//{
		//	newId := CreateDebugCall(this&)
		//	if newId != -1
		//	{
		//		f << ", !dbg !" << newId
		//	}
		//}
		//f << "\n" 
	}
	GetDeferUsage := virtual !() -> int
	{
		return 1
	}
	PrintDeferUse := virtual !(TIOStream f, BoxFuncContainer^ bd,BoxBlock^ blk, int depth,int^ labelIter) -> void
	{
		labelIter^ += 1
		curId := labelIter^
		f << "br label %DeferLabel" << curId << "\n"
		f << "DeferLabel" << curId << ":\n"
		Down.PrintInBlock(f)
	}
	DoTheWork := virtual !(int pri) -> void
	{
		//WorkBag.Push(Down,pri)

		if pri == State_Start
		{
			fnc := GetFuncBody()
			if fnc != null
			{
				fnc.DoDefer()
				WorkBag.Push(Down,State_Start)
			}
		}
	
	}
}
PrintDeferDepth := !(TIOStream f, int debId) -> void
{
	f << "%NowDepth = call i32 @" << deferGetDepth2.OutputName << "()"
	if debId != -1
	{
		f << ", !dbg !" << debId
	}
	f <<"\n"
}
PrintDeferSkip := !(TIOStream f, int debId) -> void
{
	f << "%NowDepth = call i32 @" << deferGetDepth2.OutputName << "()"
	if debId != -1
	{
		f << ", !dbg !" << debId
	}
	f <<"\n"
}
