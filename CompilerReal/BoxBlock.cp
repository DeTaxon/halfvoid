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
	gotOutRName := bool

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

		if not RNames.Empty()
		{
			f << "br label %" << RNames[RNames.Size() - 1] << "\n"
		}

		if not InClass
		{
			
			Iters := Stack.{Object^}()
			INames := Stack.{string}()

			iter = Down
			iter2 := RNames.Start

			while iter != null
			{
				Iters.Push(iter)
				INames.Push(iter2.Data)
				iter2 = iter2.Next
				iter = iter.Right
			}

			while not Iters.Empty()
			{
				itr1 := Iters.Pop()
				nms := INames.Pop()

				f << nms << ":\n"
				itr1.PrintDestructor(f)
				if not INames.Empty(){
					nxtName := INames[0]
					f << "br label %" << nxtName << "\n"
				}
			}
			f << "br label %" << outRName << "\n"

		}
	}
	LoadOutPath := !() -> void
	{
		if not gotOutRName{
			if Up != null{
				outRName = Up.GetOutPath(this&,PATH_RETURN,1)
			}else{
				EmitError("software error 311653\n")
			}
			gotOutRName = true
		}
	}
	GetOutPath := virtual !(Object^ objs, int typ , int size) -> string
	{
		if typ == PATH_RETURN
		{
			LoadOutPath()

			iter1 := Down
			iter2 := RNames.Start
			preName := outRName
			fourd := false

			while iter1 != null
			{
				if iter1 == objs
				{
					iter1 = null
					fourd = true
				}else
				{
					preName = iter2.Data
					iter1 = iter1.Right
					iter2 = iter2.Next
				}
			}
			if not fourd
			{
				EmitError("Path not found\n")
			}
			return preName
		}
		return ""
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
				RNames.Push("RName" + ItId + "n" + i)
				iter = iter.Right
				i += 1
			}
			WorkBag.Push(this&,State_DestructGet)
		}
		if pri == State_DestructGet
		{
			LoadOutPath()
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
		WorkBag.Push(this&,State_Syntax)
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
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Syntax
		{
			SyntaxCompress(this&,PriorityData)
			UnboxParams(this.Down)
			iter := this.Down
			while iter != null
			{
				WorkBag.Push(iter,State_Start)
				iter = iter.Right
			}
		}
	}
}

PATH_RETURN := 1
