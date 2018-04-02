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

			MakeItBlock(Down.Right)

			RR := Down.Right.Right
			if RR != null
				MakeItBlock(RR)
		}
		if pri == State_Syntax
		{
			iter := Down

			while iter != null
			{
				WorkBag.Push(iter,State_Start)
				iter = iter.Right
			}
			WorkBag.Push(this&,State_GetUse)
		}
		if pri == State_GetUse
		{
			if Down.GetType() != GetType("bool")
			{
				if TypeCmp(Down.GetType(),GetType("bool")) != 255
				{
					BoxExc(Down,GetType("bool"),false)
				}else{
					ErrorLog.Push("can not use type as if statement\n")
				}
			}
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

			MakeItBlock(Down.Right)
		}
		if pri == State_Syntax
		{
			iter := Down

			while iter != null
			{
				WorkBag.Push(iter,State_Start)
				iter = iter.Right
			}
			WorkBag.Push(this&,State_GetUse)
		}
		if pri == State_GetUse
		{
			if Down.GetType() != GetType("bool")
			{
				if TypeCmp(Down.GetType(),GetType("bool")) != 255
				{
					BoxExc(Down,GetType("bool"),false)
				}else{
					ErrorLog.Push("can not use type as if statement\n")
				}
			}
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
			f << "\nbr label %Check" << MyId << "\n"
			f << "End" << MyId << ":\n"

		}

	}
	GetValue := virtual !() -> string
	{
		return "~while()"
	}
}
