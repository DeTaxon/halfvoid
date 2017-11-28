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
