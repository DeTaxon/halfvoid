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

	outRName := string
	gotOutRName := bool

	usePaths := bool

	this := !() -> void
	{
		ItId = GetNewId()
	}
	this := !(Object^ toRepl) -> void
	{
		ItId = GetNewId()
		if toRepl.GetValue() == "{}"
		{	
			Down = toRepl.Down
		}else
		{
			Down = toRepl
		}
		if Down != null Down.SetUp(this&)
		Line = toRepl.Line
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
		if Line == null Line = Up.Line

		if callDeferStuf and usePaths 
			PrintDeferDepth(f,ItId,this&)	
		for iter : Down
		{
			iter.PrintInBlock(f)
		}
		if usePaths {
			f << "br label %LastContPath" << ItId << "\n"

			if askedRetPath
			{
				f << "PreRetPath" << ItId << ":\n"
				if callDeferStuf
					PrintDeferApply(f,ItId,this&)
				if gotRetPath f << "br label %" << outRName << "\n"
				
			}

			if not InClass
			{
				//if gotRetPath PrintSomePath(f,"RetPath",ItId,outRName)
				if gotRetPath f << "br label %" << outRName << "\n"
			}
			f << "LastContPath" <<ItId << ":\n"
			if callDeferStuf
				PrintDeferApply(f,ItId,this&)
		}
	}
	callDeferStuf := bool
	askedRetPath := bool
	ApplyDeferUse := virtual !(int depth) -> void
	{
		if depth != 1
		{
			Up.ApplyDeferUse(depth - 1)
		}else{
			callDeferStuf = true
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

		if typ == PATH_RETURN
		{
			LoadOutPath()

			gotRetPath = true

			if Up != null
			{
				if Up.GetValue() == "!()" or Up.GetValue() == "{!()}" or Up.GetValue() == "x=>x"
				{	
					askedRetPath = true
					return "PreRetPath" + ItId
				}
			}
			return outRName
		}
		if typ == PATH_CONTINUE
		{
			if size == 0 return "LastContPath" + ItId 
			return  Up.GetOutPath(this&,typ,size - 1)
		}
		if typ == PATH_BREAK
		{
			if size == 0 return "LastContPath" + ItId
			return Up.GetOutPath(this&,typ,size - 1)
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
				switch tstNode
				{
					case MacroRestart itr = Down
					case null itr = itr.Right
					case void itr = tstNode
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
						if itr.GetValue() == "if" and itr.Right != null and itr.Right.GetValue() == "~ind"
						{
							if itr.Right.GetValue() == "~ind"
							{
								asInd := itr.Right->{ObjIndent^}
								if asInd.MyStr[0] == '$'
								{
									inMap := TryCompute(asInd)
									if inMap == null or (inMap is ObjBool and not inMap->{ObjBool^}.MyBool)
									{
										PopOutNode(itr.Right.Right)
										PopOutNode(itr.Right)
										PopOutNode(itr)
									}else{
										//PopOutNode(itr.Right.Right)
										PopOutNode(itr.Right)
										PopOutNode(itr)
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
	//borrowed := QueueSet.{BoxFile^}
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

			//toVisit := Stack.{BoxFile^}() ; $temp
			//visited := QueueSet.{BoxFile^}() ; $temp

			//visited.Push(this&)
			//toVisit.Push(ImportingFiles[^])

			//while toVisit.NotEmpty()
			//{
			//	toTest := toVisit.Pop()

			//	if borrowed.Contain(toTest) continue
			//	
			//	borrowed.Push(toTest)
			//	for toTest.borrowed borrowed.Push(it)

			//	for toTest.ImportingFiles
			//	{
			//		if not visited.Contain(it){
			//			toVisit.Push(it)
			//			visited.Push(it)
			//		}
			//	}
			//	for preIt : toTest.VisibleParams 
			//	for preIt
			//	{
			//		VisibleParams[it.MyStr].Push(it)
			//	}
			//}
		}
	}
}

PATH_RETURN := 1
PATH_CONTINUE := 2
PATH_BREAK := 3 
