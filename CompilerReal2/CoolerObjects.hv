ObjSkobs := class extend Object
{
	Vers := int
	"this" := !(char^ st) -> void
	{
		if st == "{}" Vers = 1
		if st == "[]" Vers = 2
		if st == "()" Vers = 3
	}
	GetValue := virtual !() -> char^
	{
		if Vers == 1 return "{}"
		if Vers == 2 return "[]"
		if Vers == 3 return "()"
		return ""
	}
	GetTokenId := virtual !() -> int
	{
		switch Vers
		{
			case 3 return 46 
			case 1 return 47 
			case 2 return 48 
		}
		return 0
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			if Vers == 1
			{
				isLft := Left != null
				oldVal := Object^
				if isLft {
					oldVal = Left
				}else{
					oldVal = Right
				}
				MakeItBlock(this&)
				if oldVal != null
				{
					if isLft{
						WorkBag.Push(oldVal.Right,State_Start)
					}else{
						WorkBag.Push(oldVal.Down,State_Start)
					}
				}

			}else{
				WorkBag.Push(this&,State_Syntax)
				WorkBag.Push(this&,State_PreGetUse)
			}
		}
		if pri == State_Syntax
		{
			SyntaxCompress(this&,PriorityData)
		}
		if pri == State_PreGetUse
		{
			WorkBag.Push(Down[^],State_Start)
		}
	}
	Clone := virtual !() -> Object^
	{
		PreRet := new ObjSkobs(GetValue())
		PreRet.Line = Line

		LineI := Object^()
		
		for IterD : Down
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
		}
		if PreRet.Down != null PreRet.Down.SetUp(PreRet)

		return PreRet
	}
}


UniteSkobs := !(Object^ Tree) -> Object^
{
	Bag := new Stack.{Object^} ; $temp
	iter := Tree
	
	while iter != null
	{
		Value := iter.GetValue()
		tokId := iter.GetTokenId()

		if tokId in 40..42 	Bag.Push(iter)

		if tokId in 43..45
		{
			itPair := char^
			switch tokId
			{
				case 43 itPair = "()"
				case 44 itPair = "{}"
				case 45 itPair = "[]"
			}

			if Bag.Empty() return iter
			Old := Bag.Pop()
			if tokId - Old.GetTokenId() != 3
			{
				Old.EmitError("incorrect pair for "sbt + Old.GetValue())
				return iter
			}

			NewObj := new ObjSkobs(itPair)
			NewObj.Line = Old.Line
			UNext(Old,NewObj,iter)

			TIter := NewObj.Down
			PopOutNode(TIter)
			TIter = NewObj.Down

			if TIter.Right != null{
				while TIter.Right != null TIter = TIter.Right
			}
			PopOutNode(TIter)

			iter = NewObj.Right
		}else{
			iter = iter.Right
		}

	}
	for Bag^
	{
		it.EmitError("pair for "sbt + it.GetValue() + " not found")
	}
	return null
}
