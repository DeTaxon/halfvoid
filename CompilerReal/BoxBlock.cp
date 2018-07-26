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
	Items := Queue.{ObjHolder^}
	InClass := bool
	ItId := int

	RNames := Queue.{string}
	outRName := string

	this := !() -> void
	{
		ItId = GetNewId()
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
		if Down != null Down.SetUp(this&)
	}
	GetItem := virtual !(string name) -> Object^
	{
		iter := Items.Start
		while iter != null
		{
			if iter.Data.ItName == name return iter.Data.Down
			iter = iter.Next
		}
		return null
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

		if not InClass
		{
			iter = Down

			nowOutName := outRName

			iter2 := RNames.Start

			while iter != null
			{
				iter.PrintDestructor(f)
				f << "br label %" << outRName <<"\n"
				outRName = iter2.Data
				iter = iter.Right
				iter2 = iter2.Next
			}

		}
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			WorkBag.Push(this&,State_Syntax)
			if Up != null
			{
				InClass =  Up.GetValue() == "{...}"
			}
		}
		if pri == State_Syntax
		{
			SyntaxCompress(this&,PriorityData)
			UnboxParams(this.Down)
			WorkBag.Push(this&,State_GetUse)

			if InClass
			{
				iter := this.Down
				while iter != null
				{
					WorkBag.Push(iter,State_Start)
					iter = iter.Right
				}
			}

		}
		if pri == State_GetUse and not InClass
		{
			iter := Down

			for GetUseIter
				if iter != null iter = iter.Right
			if iter != null
			{
				WorkBag.Push(this&,State_GetUse)
				WorkBag.Push(iter,State_Start)
				GetUseIter += 1
			}else{
				WorkBag.Push(this&,State_DestructCheck)
			}
		}
		if pri == State_DestructCheck
		{
		 	iter := Down
			i := 0

			while iter != null
			{
				RNames.Push("%RName" + ItId + "n" + i)
				iter = iter.Right
				i += 1
			}
			WorkBag.Push(this&,State_DestructGet)
		}
		if pri == State_DestructGet
		{
			if Up != null{
				outRName = Up.GetOutPath(this&,PATH_RETURN,1)
			}else{
				EmitError("software error 311653\n")
			}
		}
	}
}
BoxFile := class extend BoxBlock
{
	fileId := int
	fileName := string

	returnPath := Queue.{string}

	this := !(string name) -> void
	{
		fileId = GetNewId()
		fileName = name
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

PATH_RETURN := 1
