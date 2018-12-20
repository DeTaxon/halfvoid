#import "Tree.cp"

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
			if Down != null
			{
				if Down.Right == null and Down.GetValue() == "()"
				{
					SyntaxCompress(Down,PriorityData)
					newDown := Down.Down
					if newDown != null
					{
						if newDown is ObjData and newDown.Right == null
						{
							newDown = newDown.Down
						}
						Down = newDown
						if Down != null
						{
							Down.SetUp(this&)
						}
					}else{
						EmitError("Put\n")
					}
				}
			}
		}
		if pri == State_PreGetUse
		{
			iter := Down
			while iter != null
			{
				lazy := iter.GetValue() == "."
				lazy = lazy or iter.GetValue() == "->"
				if lazy lazy = iter.Right is ObjIndent
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
									SyntaxCompress(SomeDown.Right,PriorityData)
									SizeTree = SomeDown.Right.Down
									SomeDown.Right.Up = null
									SomeDown.Right.Left = null
									SomeDown.Right = null
									iter.Right = iter.Right.Down
									iter.Right.Left = iter
									iter.Right.Up = iter.Up
									iter.Right.Right = null
								}
							}
						}
						SomeDown = iter.Right
						if SizeTree != null
						{
							iter.Right.Right = SizeTree
							SizeTree.Left = iter.Right
							SizeTree.Up = iter.Up
						}
						iter = iter.Right
					}else{
						lazy2 := iter is ObjIndent
						if lazy2 lazy2 = iter.Right != null
						if lazy2 lazy2 = iter.Right.GetValue() == "."
						if lazy2 lazy2 = iter.Right.Right != null
						if lazy2 lazy2 = iter.Right.Right.GetValue() == "{}"
						if lazy2
						{
							iter = iter.Right
						}else{
							ignore := false
							if iter.GetValue() == "()" WorkBag.Push(iter,State_Syntax)
							if iter.GetValue() == "[]" WorkBag.Push(iter,State_Syntax)
							//if iter.GetValue() == "[]" WorkBag.Push(iter,State_PreGetUse)
							if iter.GetValue() == "{}"
								if iter.Left != null
									if iter.Left.GetValue() == "." or iter.Left.GetValue() == "->"
										ignore = true
							if not ignore WorkBag.Push(iter,State_Start)
						}
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
				itType := ParseType(this&)
				if itType != null
				{
					ReplaceNode(this&,new ObjType(itType))
				}else{
					printf("this error\n")
					Print(0)
					printf("items\n")
					ir := Down
					while ir != null
					{
						if ir.GetType() != null
						{
							printf("item type %s\n",ir.GetType().GetGoodName())
						}
						ir = ir.Right
					}
					EmitError("can not parse data\n")
				}
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
