GetBoxFor := !(Object^ dat) -> BoxFor^
{

	itemName := string
	indName := string

	iterY := dat.Down.Right

	if iterY.Right.GetValue() == ","
	{
		itemName = iterY->{ObjIndent^}.MyStr
		indName = ((iterY.Right.Right)->{ObjIndent^}).MyStr
		
		iterY = iterY.Right.Right.Right
	}else
	{
		if iterY.Right.GetValue() == ":"
		{
			itemName = iterY->{ObjIndent^}.MyStr
			indName = null

			iterY = iterY.Right.Right
		}
	}
	//TODO: check type
	return new BoxForInt(itemName,indName,null->{Object^},iterY,iterY.Right)

	return null
}

BoxFor := class extend Object
{

}

BoxForInt := class extend BoxFor
{
	LocPar := LocalParam^
	thisId := int
	this := !(string f_it, string f_ind,Object^ start, Object^ end, Object^ stuf) -> void
	{	
		itemStart := start
		if start == null itemStart = new ObjInt(0)
		
		parId := GetAlloc(stuf,GetType("int"))
		LocPar = new LocalParam(GetType("int"),parId)
		
		Down = stuf
		stuf.Left = null
		stuf.Right = itemStart
		itemStart.Right = end
		itemStart.Left = stuf
		end.Left = itemStart
		end.Right = null
		Down.SetUp(this&)
		MakeItBlock(stuf)
		stuf.Up = this&

		thisId =  GetNewId()
	}
	GetValue := virtual !() -> string
	{
		return "~for()"
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			WorkBag.Push(Down,State_Start)
		}
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		stuf := Down
		beg := Down.Right
		end := beg.Right

		LocPar.PrintPointPre(f,thisId)
		beg.PrintPre(f)

		f << "store i32 " << beg.GetName() << " , i32* " << LocPar.GetPointName(thisId) << "\n"
		f << "%GoingTo" << thisId << " = add i32 " << beg.GetName() << " , 0\n"

		f << "br label %Check\n"
		f << "Check" << thisId << " :n"



	}
}

