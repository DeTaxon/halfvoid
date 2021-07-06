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
	}
	PrintInBlock := virtual !(TIOStream f) -> void
	{
	}
	GetDeferUsage := virtual !() -> int
	{
		return 1
	}
	GetDeferUsageVerticalSize := virtual !() -> int
	{
		return 1
	}
	PrintDeferUse := virtual !(TIOStream f, BoxFuncContainer^ bd,BoxBlock^ blk, int depth,int^ labelIter) -> void
	{
		curId := labelIter^
		labelIter^ -= 1
		f << "br label %DeferLabel" << curId << "\n"
		f << "DeferLabel" << curId << ":\n"

		if onExcp{
			f << "br i1 %isException , label %DeferDo" << curId << ", label %DeferEnd" << curId << "\n"
			f << "DeferDo" << curId << ":\n"
		}
		Down.PrintInBlock(f)
		if onExcp{
			f << "br label %DeferEnd" << curId << "\n"
			f << "DeferEnd"<<curId<<":\n"
		}
	}
	
	PrintDeferInBlock := virtual !(TIOStream f, int itId,int^ labelSetIter) -> void
	{
		f << "store i8 " << labelSetIter^ << " , i8* %DeferStack" << itId << "\n"
		labelSetIter^ += 1
	}
	DoTheWork := virtual !(int pri) -> void
	{
		//WorkBag.Push(Down,pri)

		if pri == State_Start
		{
			fnc := GetBoxFuncContainer(this&)
			if fnc != null
			{
				fnc.DoDefer()
				WorkBag.Push(Down,State_Start)
			}
		}
	
	}
}
PrintAddDefer := !(TIOStream f, char^ funcName,char^ ptrName,int debId) -> void
{
	f  << "call void @" << deferAddDefer2.OutputName << "(void(i8*,i1)* " << funcName << ",i8* "<< ptrName <<")"

	if debId{
		f << " , !dbg !" << debId
	}
	f << "\n"
}
PrintDeferDepth := !(TIOStream f, int itId ,int debId) -> void
{
	f << "%NowDepth"<< itId <<" = call i32 @" << deferGetDepth2.OutputName << "()"
	if debId != -1
	{
		f << ", !dbg !" << debId
	}
	f <<"\n"
}
PrintDeferSkip := !(TIOStream f, int itId, int debId) -> void
{
	f << "call void @" << deferSkip2.OutputName << "(i32 %NowDepth"<< itId<<")"
	if debId != -1
	{
		f << ", !dbg !" << debId
	}
	f <<"\n"
}
