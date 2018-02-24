ObjData := class extend Object
{
	this := !() -> void
	{
	}
	GetValue := virtual !() -> string
	{
		return "~d"
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			WorkBag.Push(this&,State_PreGetUse)
		}
		if pri == State_PreGetUse
		{
			iter := Down
			while iter != null
			{
				lazy := iter.GetValue() == "."
				if lazy lazy = iter.Right.GetValue() == "~ind"
				if lazy
				{
					iter = iter.Right
				}else
				{
					if iter.GetValue() == "()" WorkBag.Push(iter,State_Syntax)
					WorkBag.Push(iter,State_Start)
				}
				iter = iter.Right
			}
			WorkBag.Push(this&,State_GetUse)
		}
		if pri == State_GetUse
		{
			lowTest := GetUse(this&)
			if lowTest == null
			{
				ErrorLog.Push("can not parse data\n")
			}
			else
			{
				ReplaceNode(this&,lowTest)
			}
		}

	}
	Clone := virtual !() -> Object^
	{
		PreRet := new ObjData()
		PreRet.Line = Line

		IterD := Down
		LineI := Object^
		LineI = null
		
		while IterD != null
		{
			if LineI == null
			{
				PreRet.Down = IterD.Clone()
				LineI = PreRet.Down
			}else{
				LineI.Right = IterD.Clone()
				LineI.Right.Left = LineI
				LineI = LineI.Right
			}
			IterD = IterD.Right
		}
		if PreRet.Down != null PreRet.Down.SetUp(PreRet)

		return PreRet
	}
}
