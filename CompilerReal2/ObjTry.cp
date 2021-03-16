
expEnter := BoxFunc^ // AddDefer(!()&->void,bool isException)
expLeave := BoxFunc^
//expThrow := BoxFunc^
expGetPoint := BoxFunc^


ExceptionInit := !() -> bool
{


	dummy := new Object ; $temp
	box := new FuncInputBox ; $temp

	entr := FindFunc("internalEnterTry",dummy,box^,false)
	if entr == null return false
	entr.ParseBlock()
	expEnter = entr

	lev := FindFunc("internalLeaveTry",dummy,box^,false)
	if lev == null return false
	lev.ParseBlock()
	expLeave = lev

	getPr := FindFunc("internalGetExceptPoint",dummy,box^,false)
	if getPr == null return false
	getPr.ParseBlock()
	expGetPoint = getPr

	//box.itPars.Emplace(GTypeVoidP,false)
	//thro := FindFunc("internalThrow",dummy,box^,false)
	//if thro == null return false
	//thro.ParseBlock()
	//expThrow = thro

	return true
}

ParseTry := !(Object^ itm) -> Object^
{
	if itm.Down == null return null

	//if itm.Down.GetValue() == "throw" return new ObjThrow(itm)
	if itm.Down.GetValue() == "try" return new ObjTry(itm)

	return null
}

//ObjThrow := class extend Object
//{
//	ItId := int
//	this := !(Object^ itm) -> void
//	{
//		ItId = GetNewId()
//		Line = itm.Line
//	}
//	PrintInBlock := virtual !(TIOStream f) -> void
//	{
//		f << "call void @" << expThrow.OutputName << "(i8* null)"
//		if DebugMode
//		{
//			newId := CreateDebugCall(this&)
//			if newId != -1
//				f << ", !dbg !" << newId
//		}
//		f << "\n"
//	}
//}

ObjTry := class extend Object
{
	ItId := int
	exName := char^
	exType := Type^
	exParam := MemParam^
	this := !(Object^ itm) -> void
	{
		ItId = GetNewId()
		PopOutNode(itm.Down)
		PopOutNode(itm.Down.Right)
		Down = itm.Down
		Down[^].Up = this&
		WorkBag.Push(this&,State_Start)
		Line = itm.Line
	}
	DoTheWork := virtual !(int st) -> void
	{
		if st == State_Start
		{
			WorkBag.Push(this&,State_Syntax)
		}
		if st == State_Syntax
		{
			SyntaxCompress(Down.Right,PriorityData)
			WorkBag.Push(this&,State_PreGetUse)
		}
		if st == State_PreGetUse
		{
			objP := Down.Right
			if objP.Down == null
			{
				EmitError("empty catch not allowed, try \"IException^ e\"")
				return void
			}
			if objP.Down.Right == null
			{
				EmitError("one obj in catch not allowed, try \"IException^ e\"")
				return void
			}
			exType = ParseType(objP.Down)
			if exType == null or (not exType is TypePoint)
			{
				EmitError("can not parse type in catch, or is not point")
				return void
			}
			if not objP.Down.Right is ObjIndent
			{
				EmitError("second object in catch must be indentificator\n")
				return void
			}
			asInd := objP.Down.Right->{ObjIndent^}
			exName = asInd.MyStr

			exParam = new FuncParam("I"sbt + ItId,exType,false)

			MakeItBlock(Down)
			MakeItBlock(Down.Right.Right)
			WorkBag.Push(Down,State_Start)
			WorkBag.Push(Down.Right.Right,State_Start)
		}
	}
	GetItem := virtual !(char^ name) -> Object^
	{
		if name == exName
		{
			return exParam
		}
		return null
	}
	PrintInBlock := virtual !(TIOStream f) -> void
	{
		newId := -1
		if DebugMode
		{
			newId = CreateDebugCall(this&)
		}

		f << "%E" << ItId << "=  call i8* @" << expEnter.OutputName << "()"
		if newId != -1 f << ", !dbg !" << newId
		f <<"\n"


		f << "%AsArray" << ItId << " = bitcast i8* %E" << ItId << " to [20 x i8*]*\n"
  		f << "%Frame" << ItId << "  = call i8* @llvm.frameaddress.p0i8(i32 0)\n"
		f << "%FrameStore" << ItId << " = getelementptr [20 x i8*], [20 x i8*]* %AsArray"<< ItId << ", i64 0, i64 0\n"
		f << "store i8* %Frame" << ItId << ", i8** %FrameStore" << ItId << ", align 16\n"
  		f << "%Stack" << ItId << " = call i8* @llvm.stacksave()\n"
		f << "%StackStore" << ItId << " = getelementptr [20 x i8*], [20 x i8*]* %AsArray"<< ItId << ", i64 0, i64 2\n"
  		f << "store i8* %Stack" << ItId << ", i8** %StackStore"<< ItId <<", align 16\n"
  		f << "%T" << ItId << " = call i32 @llvm.eh.sjlj.setjmp(i8* %E" << ItId <<")\n"
		//f << "%T" << ItId << " = call i32 @setjmp(i8* %E" << ItId << ")"
		//if newId != -1 f << ", !dbg !" << newId
		f << "\n"

		f << "%B" << ItId << "= icmp eq i32 %T" << ItId << " , 0\n"
		f << "br i1 %B" << ItId << ", label %OnGood" << ItId << " , label %OnBad" << ItId << "\n"
		f << "OnGood" << ItId << ":\n"
		Down.PrintInBlock(f)
		f << "br label %OnEnd" << ItId << "\n"
		f << "OnBad" << ItId << ":\n"

		if exName != null
		{
			f << "%IPre" << ItId << " = call i8* @" << expGetPoint.OutputName << "()"
			if newId != -1 f << ", !dbg !" << newId
			f << "\n"
			f << "%I" << ItId << " = bitcast i8* %IPre" << ItId << " to " << exType.GetName() << "\n"
		}

		Down.Right.Right.PrintInBlock(f)
		f << "br label %OnEnd" << ItId << "\n"
		f << "OnEnd" << ItId << ":\n"
		f << "call void @" << expLeave.OutputName << "()"
		if newId != -1
		{
			f << ", !dbg !" << newId
		}
		"\n"
	}
}
