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


GetBlock := !(Object^ obj) -> BoxBlock^
{
	itr := obj
	while itr != null
	{
		if itr is BoxBlock
			return itr->{BoxBlock^}
		itr = itr.Up
	}
	return null
}

BoxBlock := class extend Object
{
	GetUseIter := int
	Items := List.{ObjHolder^}
	InClass := bool
	ItId := int
	
	gotRetPath := bool

	outRLabel := BoxLabel^

	usePaths := bool

	gcObjects := HybridQueue.{int,8}
	AddGC := !(int varId) -> void
	{
		gcObjects.Push(varId)
	}
	//PrintCleanGC := !(TIOStream f) -> void //TODO WORK
	//{
	//	if gcObjects.Size() == 0
	//		return void
	//	aBox := AllocBox^()
	//	itr := Up
	//	while itr != null
	//	{
	//		aBox = itr.GetABox()
	//		if aBox != null
	//			break
	//		itr = itr.Up
	//	}
//
	//	for it : gcObjects
	//	{
	//		typ := aBox.GetObjType(it)
	//		
	//		f << "store "
	//		typ.PrintType(f)
	//		f << " null, "
	//		typ.PrintType(f)
	//		f << "* %T" << it << "\n"
	//	}
	//	f << "%StackObj" << ItId << " = bitcast " << aBox.GetAsUse() << " to i8*\n"
//
	//	f << "call void @" << deferAddDefer.OutputName << "(void(i8*)* @BlockGCCleanUp"<<ItId <<" , i8* %StackObj"<< ItId <<" )"
	//	if DebugMode
	//	{
	//		if Line == null and Up?.Line != null
	//			Line = Up.Line
//
	//		cllId := CreateDebugCall(this&)
	//		if cllId != -1
	//		{
	//			f << ", !dbg !" << cllId
	//		}
	//	}
	//	f << "\n"
	//}

	this := !() -> void
	{
		ItId = GetNewId()
	}
	this := !(Object^ toRepl) -> void
	{
		quitPath."this"()
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
			woot := Queue.{Object^}() ; $temp

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
	GetScope := virtual !() -> int { 
		if Up == null or Up.GetScope() == -1
			return -1
		return ItId 
	}
	PrintGlobal := virtual !(TIOStream f) -> void 
	{
		if callDeferStuf
		{
			f << "define void @BlockDeferCall" << ItId << "(i8* %StackObj,i1 %isException)\n"
			f << "{\n"
			f << "	ret void\n"
			f << "}\n"
		}

		if gcObjects.Size() != 0
		{
			aBox := AllocBox^()
			itr := Up
			while itr != null
			{
				aBox = itr.GetABox()
				if aBox != null
					break
				itr = itr.Up
			}

			f << "define void @BlockGCCleanUp" << ItId << "(i8* %StackObj)\n"
			f << "{\n"
			f << "%AllcObj = bitcast i8* %StackObj to " << aBox.GetClassName() << "*\n"
			aBox.PrintBoxItems(f,"%AllcObj",-1)
			for it,i : gcObjects
			{
				typ := aBox.GetObjType(it)
				tn := typ.GetName()
				f << "br label %Start" << i << "\n"
				f << "Start" << i << ":\n"
				f << "%LD" << it << " = load " << tn << " , " << tn << "* %T" << it << "\n"
				f << "%AsVoidP" << i << " = bitcast " << tn << " %LD"<<it<<" to i8*\n"
				f << "call void @" << gcDecRefFunc.OutputName << "(i8* %AsVoidP" << i << ")\n"
				f << "store " << tn << " null, " << tn << "* %T" << it << "\n"
				f << "br label %End" << i << "\n"
				f << "End"<<i<<":\n"
			}
			f << "ret void\n}\n"
		}
		PrintGlobalSub(f)
		if DebugMode
		{
			itr := Up
			while itr.Up != null itr = itr.Up
			if itr is BoxFile
			{
				f << "!" << ItId << " = !DILexicalBlock(scope: !" 
				f << Up.GetScope() << ", file: !" << itr->{BoxFile^}.fileId
				f << ")\n"
			}
//!20 = distinct !DILexicalBlock(scope: !16, file: !1, line: 6, column: 2)
		}
	}
	PrintSomePath := !(TIOStream f, string PathName, int num,string OutName) -> void
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
	PrintInBlock := virtual !(TIOStream f) -> void
	{
		if Line == null Line = Up.Line

			
		//PrintCleanGC(f) //TODO
		for iter : Down
		{
			iter.PrintInBlock(f)
		}
		if usePaths {
			f << "br label %" << quitPath.GetLabel() << "\n"

			if preRetLabel != null
			{
				preRetLabel.PrintLabel(f)
				if gotRetPath f << "br label %" << outRLabel.GetLabel() << "\n"
				
			}

			if not InClass
			{
				//if gotRetPath PrintSomePath(f,"RetPath",ItId,outRName)
				if gotRetPath f << "br label %" << outRLabel.GetLabel() << "\n"
			}
			//f << "LastContPath" <<ItId << ":\n"
			quitPath.PrintLabel(f)
		}
	}
	callDeferStuf := bool
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
		if outRLabel == null{
			if Up != null{
				outRLabel = Up.GetOutPath(this&,PATH_RETURN,1)
			}else{
				EmitError("software error 311653\n")
			}
		}
	}
	preRetLabel := BoxLabel^
	quitPath := BoxLabelAnon
	GetOutPath := virtual !(Object^ objs, int typ , int size) -> BoxLabel^
	{
		if not usePaths
		{
			if Up == null return null
			return Up.GetOutPath(objs,typ,size)
		}

		if typ == PATH_RETURN
		{
			LoadOutPath()

			gotRetPath = true

			if Up?.GetValue() in !["!()","{!()}","x=>x"]
			{	
				if preRetLabel == null preRetLabel = new BoxLabelAnon()
				return preRetLabel
			}
			return outRLabel
		}
		if typ == PATH_BREAK || typ == PATH_CONTINUE
		{
			if size == 0 {
				return quitPath&
			}
			return Up.GetOutPath(objs,typ,size - 1)
		}
		return null
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			WorkBag.Push(this&,State_Syntax)
			if Up != null
			{
				if not InClass
					InClass =  IsClassObj(Up)
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
				if (not itr is ObjData) and itr.GetValue() != "?or??" and itr.GetValue() != "return()" and not (itr is QJumpLand){ //TODO: wut is dat
				//if (not itr is ObjData) and itr.GetValue() != "?or??"{
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
						if itr.GetValue() == "if" and itr.Right?.GetValue() == "~ind"
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
	GetScope := virtual !() -> int
	{
		return fileId
	}
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
	PrintGlobal := virtual !(TIOStream f) -> void
	{
		Down[^].PrintGlobal(f)
		if DebugMode //later
		{
			f << "!" << fileId  << " = !DIFile(filename: \"" 
			f << filePath.FullPath().itStr << "\", directory: \""
			f <<  "/" <<"\")\n"
		}
	}
	PrintInBlock := virtual !(TIOStream f) -> void
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

			itr := Down
			while itr != null
			{
				if itr.Down?.GetValue() == "AppendClass"
				{
					oldDown := itr.Down
					itr = ReplaceNode(itr,new BoxClassAppend())
					itr.Down = oldDown
					itr.Down.SetUp(itr)
				}
				itr = itr.Right
			}
			WorkBag.Push(Down[^],State_Start)
		}
		if pri == State_BlockParamStep
		{
			if cs != null
			{
				for v,k : VisibleParams
				{
					toAdd := ref cs.codeParams[k]
					toAdd.Push(v[^])
				}
			}
		}
	}
}

PATH_RETURN := 1
PATH_CONTINUE := 2
PATH_BREAK := 3 
