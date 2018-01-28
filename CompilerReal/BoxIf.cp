BoxIf := class extend Object
{
	MyId := int
	this := !(Object^ DaRet) -> void
	{
		Down = DaRet.Down
		MyId = GetNewId()
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			WorkBag.Push(this&,State_Syntax)
			Down.SetUp(this&)
			PopOutNode(Down)
			if Down.Right.Right != null PopOutNode(Down.Right.Right)

			ReplaceNode(Down.Right,new BoxBlock(Down.Right))

			RR := Down.Right.Right
			if RR != null
				ReplaceNode(RR,new BoxBlock(RR))
		}
		if pri == State_Syntax
		{
			iter := Down

			while iter != null
			{
				WorkBag.Push(iter,State_Start)
				iter = iter.Right
			}
			//WorkBag.Push(this&,State_GetUse)
		}
		if pri == State_GetUse
		{
		}
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		if Down.Right.Right == null
		{
			Down.PrintPre(f)
			f << "br "
			Down.PrintUse(f)
			f << ", label %OnTrue" << MyId << " , label %End" << MyId << "\n"
			f << "OnTrue" << MyId << ":\n"
			Down.Right.PrintInBlock(f)
			f << "br label %End" << MyId << "\n"
			f << "End" << MyId << ":\n"

		}else{
			Down.PrintPre(f)
			f << "br "
			Down.PrintUse(f)
			f << ", label %OnTrue" << MyId << " , label %OnFalse" << MyId << "\n"
			f << "OnTrue" << MyId << ":\n"
			Down.Right.PrintInBlock(f)
			f << "br label %End" << MyId << "\n"
			f << "OnFalse" << MyId << ":\n"
			Down.Right.Right.PrintInBlock(f)
			f << "br label %End" << MyId << "\n"
			f << "End" << MyId << ":\n"
		}

	}
	GetValue := virtual !() -> string
	{
		return "~if()"
	}
}

BoxWhile := class extend Object
{
	MyId := int
	this := !(Object^ DaRet) -> void
	{
		Down = DaRet.Down
		MyId = GetNewId()
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			WorkBag.Push(this&,State_Syntax)
			Down.SetUp(this&)
			PopOutNode(Down)

			ReplaceNode(Down.Right,new BoxBlock(Down.Right))
		}
		if pri == State_Syntax
		{
			iter := Down

			while iter != null
			{
				WorkBag.Push(iter,State_Start)
				iter = iter.Right
			}
			//WorkBag.Push(this&,State_GetUse)
		}
		if pri == State_GetUse
		{
		}
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		if Down.Right.Right == null
		{
			f << "br label %Check" << MyId << "\n"
			f << "Check" << MyId << ":\n"
			Down.PrintPre(f)
			f << "br "
			Down.PrintUse(f)
			f << ", label %OnTrue" << MyId << " , label %End" << MyId << "\n"
			f << "OnTrue" << MyId << ":\n"
			Down.Right.PrintInBlock(f)
			f << "br label %Check" << MyId << "\n"
			f << "End" << MyId << ":\n"

		}

	}
	GetValue := virtual !() -> string
	{
		return "~while()"
	}
}
