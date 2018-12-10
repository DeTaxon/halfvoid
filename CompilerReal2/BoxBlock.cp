#import "Tree.cp"
#import "set.cp"
#import "Globals.cp"

MakeItBlock := !(Object^ item) -> bool
{
	return MakeItBlock(item,true)
}
MakeItBlock := !(Object^ item,bool useStuf) -> bool
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

	bloc.usePaths = useStuf

	return true
}



BoxBlock := class extend Object
{
	GetUseIter := int
	Items := Queue.{ObjHolder^}
	InClass := bool
	ItId := int
	
	gotRetPath := bool
	ContinuePath := Set.{int}
	BreakPath := Set.{int}

	outRName := string
	gotOutRName := bool

	usePaths := bool

	this := !() -> void
	{
		ItId = GetNewId()
		ContinuePath.Push(0)
	}
	this := !(Object^ toRepl) -> void
	{
		ItId = GetNewId()
		ContinuePath.Push(0)
		if toRepl.GetValue() == "{}"
		{	
			Down = toRepl.Down
		}else
		{
			Down = toRepl
		}
		if Down != null Down.SetUp(this&)
	}
	RecursMake := !() -> void
	{
			woot := Queue.{Object^}()

			itrr := Down
			while itrr != null
			{
				if itrr.GetValue() == "{}"
				{
					woot.Push(itrr)
				}
				itrr = itrr.Right
			}
			for woot
			{
				MakeItBlock(it)
			}
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
	PrintSomePath := !(sfile f, string PathName, int num,string OutName) -> void
	{
		i := 0	
		iter := Down

		while iter != null
		{
			//f << "br label %" << PathName << num << "in" << i << "\n"	
			f << PathName << num << "in" << i << ":\n"
			if UseDestructors iter.PrintDestructor(f)
			if i == 0{
				f << "br label %" << OutName << "\n"
			}else{
				f << "br label %" << PathName << num << "in" << i - 1 << "\n"
			}
			i += 1
			iter = iter.Right
		}
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		iter := Down
		i := 0
		while iter != null
		{
			iter.PrintInBlock(f)
			i += 1
			iter = iter.Right
		}
		if usePaths {
			if i != 0 f << "br label %ContPath" << ItId << "id0in" << i - 1 << "\n"
			f << "br label %LastContPath" <<ItId << "\n"

			if not InClass
			{
				if gotRetPath PrintSomePath(f,"RetPath",ItId,outRName)
				for siz : ContinuePath
				{
					retCPath := "LastContPath" + ItId
					if siz != 0 retCPath = Up.GetOutPath(this&,PATH_CONTINUE,siz - 1)
					PrintSomePath(f,"ContPath" + ItId + "id",siz,retCPath)
				}
				for siz : BreakPath
				{
					retCPath := "LastContPath" + ItId
					if siz != 0 retCPath = Up.GetOutPath(this&,PATH_BREAK,siz - 1)
					PrintSomePath(f,"BreakPath" + ItId + "id",siz,retCPath)
				}
			}
			f << "LastContPath" <<ItId << ":\n"
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
		if not usePaths
		{
			if Up == null return ""
			return Up.GetOutPath(this&,typ,size)
		}

		iter1 := Down
		i := 0
		found := false

		while iter1 != null
		{
			if iter1 == objs
			{
				iter1 = null
				found = true
			}else
			{
				i += 1
				iter1 = iter1.Right
			}
		}

		if not found
		{
			EmitError("Path not found\n")
			return ""
		}

		if typ == PATH_RETURN
		{
			LoadOutPath()

			gotRetPath = true
			if i == 0 return outRName
			if Up != null Up.GetOutPath(this&,typ,0)
			return "RetPath" + ItId + "in" + (i - 1)
		}
		if typ == PATH_CONTINUE
		{
			ContinuePath.Insert(size)
			if i == 0 {
				if size == 0 return "LastContPath" + ItId
				return Up.GetOutPath(this&,typ,size - 1)
			}
			return "ContPath" + ItId + "id"  + size + "in" + (i - 1)
		}
		if typ == PATH_BREAK
		{
			BreakPath.Insert(size)
			//if i == 0 { should not be here
			//	if size == 0 return "LastContPath" + ItId
			//	return Up.GetOutPath(this&,typ,size - 1)
			//}
			if i == 0 {
				return Up.GetOutPath(this&,typ,size - 1)
			}
			return "BreakPath" + ItId + "id"  + size + "in" + (i - 1)
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
			//if Up.GetValue() == "~switch()" RecursMake()
			//if (not InClass and (Up.GetValue() == "{d}" or Up.GetValue() == "~switch()")) RecursMake()
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
			WorkBag.Push(this&,State_DestructGet)
		}
		if pri == State_DestructGet
		{
		}
	}
}
BoxFile := class extend BoxBlock
{
	fileId := int
	filePath := Path

	returnPath := Queue.{string}

	this := !(Path fullPath) -> void
	{
		fileId = GetNewId()
		filePath.itStr = fullPath.itStr
		WorkBag.Push(this&,State_Syntax)
	}
	GetValue := virtual !() -> string
	{
		return "{d}.cp"
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
		iter := Down
		while iter != null
		{
			iter.PrintGlobal(f)
			iter = iter.Right
		}
		if DebugMode //later
		{
			f << "!" << fileId  << " = !DIFile(filename: \"" 
			f << filePath.FullPath().itStr << "\", directory: \""
			f <<  "/" <<"\")\n"
		}
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		iter := Down
		while iter != null
		{
			iter.PrintInBlock(f)
			iter = iter.Right
		}
		if DebugMode //later
		{
			f << "!" << fileId  << " = !DIFile(filename: \"" 
			f << filePath.FullPath().itStr << "\", directory: \""
			f <<  "/" <<"\")\n"
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
PATH_CONTINUE := 2
PATH_BREAK := 3 
