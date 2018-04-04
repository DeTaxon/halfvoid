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
					if iter.GetValue() == "new"
					{
						SomeDown := iter.Right.Down
						SizeTree := Object^
						SizeTree = null
						if SomeDown != null
						{
							if SomeDown.Right != null
							{	
								if SomeDown.Right.GetValue() == "[]"
								{
									SizeTree = SomeDown.Right.Down
									PopOutNode(SomeDown.Right)
									iter.Right.Right = SomeDown
									SomeDown.Left = iter.Right
									SizeTree.SetUp(SomeDown.Up)
								}
							}
						}
						SomeDown = iter.Right
						if SizeTree != null
						{
							SomeDown.Right = SizeTree
							SizeTree.Left = SomeDown
							WorkBag.Push(SizeTree,State_Start)
						}
						iter = iter.Right
					}else{
						if iter.GetValue() == "()" WorkBag.Push(iter,State_Syntax)
						WorkBag.Push(iter,State_Start)
					}
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
				err := "can not parse data"
				if Line != null{
					err = err + Line.GetLog()	
				}
				err =err + "\n"
				ErrorLog.Push(err)
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
