MakeItBlock := !(Object^ item) -> bool
{
	if item == null return false
	if item.GetValue() == "{d}" return true

	oldUp := item.Up
	oldRight := item.Right
	oldLeft := item.Left

	item.Up = null
	item.Right = null
	item.Left = null

	bloc := new BoxBlock(item)

	item.Up = oldUp
	item.Right = oldRight
	item.Left = oldLeft

	ReplaceNode(item,bloc)

	item.SetUp(bloc)

	return true
}

BoxBlock := class extend Object
{
	GetUseIter := int
	this := !() -> void
	{
	}
	this := !(Object^ toRepl) -> void
	{
		if toRepl.GetValue() == "{}"
		{	
			Down = toRepl.Down
		}else
		{
			Down = toRepl
		}
		Down.SetUp(this&)
	}
	GetValue := virtual !() -> string
	{
		return "{d}"
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		iter := Down
		while iter != null
		{
			iter.PrintInBlock(f)
			iter = iter.Right
		}
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			WorkBag.Push(this&,State_Syntax)
		}
		if pri == State_Syntax
		{
			SyntaxCompress(this&,PriorityData)
			UnboxParams(this.Down)
			WorkBag.Push(this&,State_GetUse)
		}
		if pri == State_GetUse
		{
			iter := Down

			for GetUseIter
				if iter != null iter = iter.Right
			if iter != null
			{
				WorkBag.Push(this&,State_GetUse)
				WorkBag.Push(iter,State_Start)
				GetUseIter += 1
			}
		}
	}
}
BoxFile := class extend BoxBlock
{
	fileId := int
	fileName := string
	this := !() -> void
	{
		fileId = GetNewId()
	}
	this := !(Object^ toRepl) -> void
	{
		fileId = GetNewId()
		if toRepl.GetValue() == "{}"
		{	
			Down = toRepl.Down
		}else
		{
			Down = toRepl
		}
		Down.SetUp(this&)
	}
	GetValue := virtual !() -> string
	{
		return "{d}.cp"
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		iter := Down
		while iter != null
		{
			iter.PrintInBlock(f)
			iter = iter.Right
		}
	}
}
