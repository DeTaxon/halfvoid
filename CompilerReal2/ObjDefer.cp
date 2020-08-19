deferAddDefer := BoxFunc^ // AddDefer(!()&->void,bool isException)
deferAddDeferExcp := BoxFunc^ // AddDefer(!()&->void,bool isException)
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
	depth.ParseBlock()
	deferGetDepth = depth

	box.itPars.Emplace(defFunc.GetPoint(),false)
	box.itPars.Emplace(GTypeVoidP,false)

	itFunc := FindFunc("internalDeferAdd",dummy,box^,false)
	if itFunc == null return false
	itFunc.ParseBlock()
	deferAddDefer = itFunc

	itFuncE := FindFunc("internalDeferExcpAdd",dummy,box^,false)
	if itFuncE == null return false
	itFuncE.ParseBlock()
	deferAddDeferExcp = itFuncE

	box2 := new FuncInputBox ; $temp
	box2.itPars.Emplace(GTypeInt,false)
	applyFunc := FindFunc("internalDeferApply",dummy,box2^,false)
	if applyFunc == null
		return false
	applyFunc.ParseBlock()
	
	deferApply = applyFunc

	return true
}

ObjDefer := class extend Object
{
	onExcp := bool
	this := !(Object^ dwn,bool iExp) -> void
	{
		onExcp = iExp
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

		callAdd := deferAddDefer
		if onExcp
			callAdd = deferAddDeferExcp
		itr := Up
		while itr.GetABox() == null
		{
			itr = itr.Up
		}
		asWrap := Down->{WrappedFunc^}
		funcAl := itr.GetABox()
		if funcAl.ItemBag.IsEmpty()
		{
			f << "call void @" << callAdd.OutputName << "(void(i8*)* @" << asWrap.OutputName << " , i8* null )"
		}else{
			neId := GetNewId()
			f << "%T" << neId << " = bitcast " << funcAl.GetAsUse() << " to i8*\n"
			f << "call void @" << callAdd.OutputName << "(void(i8*)* @" << asWrap.OutputName << " , i8* %T"<< neId << " )"
		}
		if DebugMode
		{
			newId := CreateDebugCall(this&)
			if newId != -1
			{
				f << ", !dbg !" << newId
			}
		}
		f << "\n" 
	}
	DoTheWork := virtual !(int pri) -> void
	{
		WorkBag.Push(Down,pri)
		if Up != null
		{
			Up.ApplyDeferUse(1)
			AllocSetStruct(Up)
		}
	}
}
PrintDeferDepth := !(sfile f, int SomeId, Object^ itm) -> void
{
	f << "%NowDepth" << SomeId << " = call i32 @" << deferGetDepth.OutputName << "()"
	if DebugMode
	{
		newId := CreateDebugCall(itm)
		if newId != -1
		{
			f << ", !dbg !" << newId
		}
	}
	f <<"\n"
}

PrintDeferApply := !(sfile f, int SomeId, Object^ itm) -> void
{
	f << "call void @" << deferApply.OutputName << "(i32 %NowDepth" << SomeId << ")"
	if DebugMode
	{
		newId := CreateDebugCall(itm)
		if newId != -1
		{
			f << ", !dbg !" << newId
		}
	}
	f <<"\n"
}
