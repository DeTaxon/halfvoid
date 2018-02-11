ObjSkobs := class extend Object
{
	Vers := int
	"this" := !(char^ st) -> void
	{
		Clean()
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
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			WorkBag.Push(this&,State_PreGetUse)
		}
		if pri == State_Syntax
		{
			SyntaxCompress(this&,PriorityData)
		}
		if pri == State_PreGetUse
		{
			iter := Down

			while iter != null
			{
				WorkBag.Push(iter,State_PreGetUse)
				iter = iter.Right
			}
		}
	}
	Clone := virtual !() -> Object^
	{
		PreRet := new ObjSkobs(GetValue())
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

UniteSkobs := !(Object^ Tree) -> Object^
{
	Bag := Stack.{Object^}()
	iter := Tree
	
	while iter != null
	{
		Value := iter.GetValue()

		if Value[0] in "{[(" 	Bag.Push(iter)

		if Value[0] in "}])"
		{
			Pair := char^
			if Value[0] == '}' Pair = "{}"
			if Value[0] == ']' Pair = "[]"
			if Value[0] == ')' Pair = "()"

			if Bag.Empty() return iter
			Old := Bag.Pop()
			if Old.GetValue()[0] != Pair[0] return iter

			NewObj := new ObjSkobs(Pair)
			UNext(Old,NewObj,iter)

			TIter := NewObj.Down
			Remove(TIter)
			TIter = NewObj.Down

			if TIter.Right != null{
				while TIter.Right != null TIter = TIter.Right
			}
			Remove(TIter)

			iter = NewObj.Right
		}else{
			iter = iter.Right
		}

	}
	if Bag.NotEmpty() return Bag.Pop()
	return null
}
