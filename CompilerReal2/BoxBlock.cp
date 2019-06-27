#import "Tree.cp"
#import "Globals.cp"
#import "GlobalParams.cp"
#import "Macro.cp"

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
		ContinuePath.Add(0)
	}
	this := !(Object^ toRepl) -> void
	{
		ItId = GetNewId()
		ContinuePath.Add(0)
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

			if Down[^].GetValue() == "{}"
				woot.Push(it)
			MakeItBlock(woot[^])
	}
	GetItem := virtual !(string name) -> Object^
	{
		if Items[^].ItName == name 
			return it.Down
		return null
	}
	GetValue := virtual !() -> string
	{
		return "{d}"
	}
	PrintSomePath := !(sfile f, string PathName, int num,string OutName) -> void
	{
		for iter,i : Down
		{
			//f << "br label %" << PathName << num << "in" << i << "\n"	
			f << PathName << num << "in" << i << ":\n"
			iter.PrintDestructor(f)
			if i == 0{
				f << "br label %" << OutName << "\n"
			}else{
				f << "br label %" << PathName << num << "in" << i - 1 << "\n"
			}
		}
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		i := 0
		for iter : Down
		{
			iter.PrintInBlock(f)
			i += 1
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

		i := 0
		found := false
		for Down
		{
			if it == objs
			{
				found = true
				break
			}
			i += 1
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
			return "RetPath"sbt + ItId + "in" + (i - 1)
		}
		if typ == PATH_CONTINUE
		{
			ContinuePath.Add(size)
			if i == 0 {
				if size == 0 return "LastContPath" + ItId
				return Up.GetOutPath(this&,typ,size - 1)
			}
			return "ContPath"sbt + ItId + "id"  + size + "in" + (i - 1)
		}
		if typ == PATH_BREAK
		{
			BreakPath.Add(size)
			//if i == 0 { should not be here
			//	if size == 0 return "LastContPath" + ItId
			//	return Up.GetOutPath(this&,typ,size - 1)
			//}
			if i == 0 {
				return Up.GetOutPath(this&,typ,size - 1)
			}
			return "BreakPath"sbt + ItId + "id"  + size + "in" + (i - 1)
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

			itr := Down
			while itr != null
			{
				if not itr is ObjData and not itr.GetValue() == "?or??"{
					itr = itr.Right
					continue
				}
				tstNode := TryParseMacro(itr)
				if tstNode != null{
					itr = tstNode
				}else{
					itr = itr.Right
				}
			}

			if InClass
			{
				clsItm := Up->{BoxClass^}
				resvItm := true

				while resvItm 
				{
					resvItm = false

					for itr : Down
					{
						if itr.GetValue() == "if()"
						{
							if itr.Down.Right.GetValue() == "~ind"
							{
								asInd := itr.Down.Right->{ObjIndent^}
								if asInd.MyStr[0] == '$'
								{
									inMap := clsItm.ItAttrs.TryFind(asInd.MyStr)
									itmToPut := Object^()
									if inMap != null
									{
										itmToPut = itr.Down.Right
									}else{
										if itr.Down.Right.Right.Right != null
											itmToPut = itr.Down.Right.Right.Right.Right
									}
									if itmToPut == null
										PopOutNode(itr)
									else{
										ReplaceNode(itr,itmToPut)
										UnboxParams(itmToPut)
									}


									
								}
							}
							resvItm = true
							break
						}
					}
				}
				
			}

			if InClass
			{
				WorkBag.Push(Down[^],State_Start)
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
	
	ImportingFiles := Queue.{BoxFile^}
	borrowed := QueueSet.{BoxFile^}
	VisibleParams := AVLMap.{string,QueueSet.{ObjParam^}}

	cs := CodeSpace^

	this := !(Path fullPath) -> void
	{
		fileId = GetNewId()
		filePath.itStr = fullPath.itStr
		WorkBag.Push(this&,State_Syntax)
		WorkBag.Push(this&,State_BlockParamStep)
	}
	GetValue := virtual !() -> string
	{
		return "{d}.cp"
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
		Down[^].PrintGlobal(f)
		if DebugMode //later
		{
			f << "!" << fileId  << " = !DIFile(filename: \"" 
			f << filePath.FullPath().itStr << "\", directory: \""
			f <<  "/" <<"\")\n"
		}
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		Down[^].PrintInBlock(f)
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
			WorkBag.Push(Down[^],State_Start)
		}
		if pri == State_BlockParamStep
		{
			//Fnd := false
			//if ForcedLibs[^] == this& {
			//	Fnd = true
			//	break
			//}
			//if Fnd{
			//	for v,k : VisibleParams
			//	{
			//		toAdd := ref ForcedGlobalParams[k]
			//		for v toAdd.Push(it)
			//	}
			//}

			if cs != null
			{
				for v,k : VisibleParams
				{
					toAdd := ref cs.codeParams[k]
					toAdd.Push(v[^])
				}
			}

			toVisit := Stack.{BoxFile^}() ; $temp
			visited := QueueSet.{BoxFile^}() ; $temp

			visited.Push(this&)
			toVisit.Push(ImportingFiles[^])

			while toVisit.NotEmpty()
			{
				toTest := toVisit.Pop()

				if borrowed.Contain(toTest) continue
				
				borrowed.Push(toTest)
				for toTest.borrowed borrowed.Push(it)

				for toTest.ImportingFiles
				{
					if not visited.Contain(it){
						toVisit.Push(it)
						visited.Push(it)
					}
				}
				for preIt : toTest.VisibleParams 
				for preIt
				{
					VisibleParams[it.MyStr].Push(it)
				}
			}
		}
	}
}

PATH_RETURN := 1
PATH_CONTINUE := 2
PATH_BREAK := 3 
