
AllocBox := class 
{
	ItId := int
	liveOnGlobal := bool
	mustBeStruct := bool

	parentAlloc := AllocBox^
	inhAllocs := List.{AllocBox^}


	ItemBag := List.{Tuple.{int,Type^}}
	ItemNrs := AVLMap.{int,int}
	GetAlloc := !(Type^ ToAdd) -> int
	{
		DaID := GetNewId()
		ItemNrs[DaID] = ItemBag.Size()
		ItemBag.Emplace(DaID,ToAdd)
		return DaID
	}
	GetClassName := !() -> string
	{
		return StrCopy("%AllocClass"sbt + ItId)
	}
	GetNR := !(int id) -> int
	{
		inNrs := ItemNrs.TryFind(id)
		if inNrs != null
			return inNrs^
		return -1
	}
	MoveTo := !(AllocBox^ toMv) -> void
	{
		toMv.inhAllocs.Push(this&) ; $uniq
		parentAlloc = toMv
	}
	InheritNR := !() -> int
	{
		cntr := parentAlloc.ItemBag.Size()
		for parentAlloc.inhAllocs
		{
			if it == this&
				break
			if it.ItemBag.Size() != 0
				cntr += 1
		}
		return cntr
	}
	//ReturnAlloc := !()
	GetAsType := !() -> Type^
	{
		resR := new FuncInputBox() ; $temp
		assert(not ItemBag.IsEmpty())
		resR.itPars.Emplace(ItemBag[^].1,false)
		for inhAllocs
		{
			resR.itPars.Emplace(it.GetAsType(),false)
		}
		return GetTuple(resR).ClassType
	}

	printedGlobal := bool
	PrintGlobal := !(TIOStream f) -> void
	{
		if printedGlobal
			return void
		printedGlobal = true 

		inhP := false
		inhP = inhP or  not inhAllocs[^].ItemBag.IsEmpty()
		if (not ItemBag.IsEmpty()) or inhP
		{
			f << "%AllocClass" << ItId << " = type {"
		
			prntDot := false
			for it,i : ItemBag
			{
				if prntDot
					f << " , "
				f << it.1.GetName()
				prntDot = true
			}
			if not inhAllocs[^].ItemBag.IsEmpty()
			{
				if prntDot 
					f << " , "
				f << it.GetClassName()
				prntDot = true
			}



			f << "}\n"
			if liveOnGlobal 
			{
				f << "@AllocObject" << ItId << " = global %AllocClass" << ItId << " zeroinitializer\n"
			}
		}
		inhAllocs[^].PrintGlobal(f)
	}
	GetAsUse := !() -> string
	{
		if ItemBag.Size() == 0
			return "%AllocClass"sbt + ItId + "* null"
		return "%AllocClass"sbt + ItId + "* %AllocItem" + ItId
	}
	PrintAlloc := !(TIOStream f,int debId) -> void
	{
		PrintAlloc(f,null->{char^},debId)
	}
	PrintAlloc := !(TIOStream f,char^ prntName,debId) -> void
	{
		if not ItemBag.IsEmpty()
		{
			if not mustBeStruct
			{
				for it : ItemBag
				{
					f << "%T" << it.0 <<" = alloca " << it.1.GetName() 
					if debId != -1
						f << ", !dbg !" << debId
					f << "\n"
				}
				return void
			}
			if liveOnGlobal
			{
				f << "%AllocItem" << ItId << " = getelementptr %AllocClass" << ItId << " , %AllocClass"
					<< ItId << "* @AllocObject" << ItId << ", i32 0"
				//if DebugMode
				//{
				//	newId := CreateDebugCall(dbgObj)
				//	if newId != -1
				//		f << " , !dbg !" << newId
				//}
				f << "\n"
			}else{
				if parentAlloc != null
				{
					assert(prntName != null)
					cntr := InheritNR()
					f << "%AllocItem" << ItId << " = getelementptr %AllocClass" << parentAlloc.ItId << " , "
						f << " %AllocClass" << parentAlloc.ItId << "* " << prntName
						f << ", i32 0, i32 " << cntr 
					//if DebugMode
					//{
					//	newId := CreateDebugCall(dbgObj)
					//	if newId != -1
					//		f << " , !dbg !" << newId
					//}
					f << "\n"
				}else{
					f << "%AllocItem" << ItId << " = alloca %AllocClass" << ItId
					if debId != -1
						f << ", !dbg !" << debId
					f << "\n"
				}
			}
		}

		PrintBoxItems(f,"%AllocItem"sbt + ItId,debId)
	}
	PrintBoxItems := !(TIOStream f,string Name,int debId) -> void
	{
		if not mustBeStruct
			return void

		for it,i : ItemBag
		{
			f << "%T" << it.0 <<" = getelementptr %AllocClass" << ItId << " , %AllocClass" << ItId << "* " << Name
			f << " , i32 0, i32 " << i
			if DebugMode and debId != -1
			{
				f << " , !dbg !" << debId
			}
			f << "\n"
		}
	}
}

GetAllocNR := !(Object^ Start,int id) -> int
{
	iter := Start
	while iter != null
	{
		ab := iter.GetABox()
		if ab != null return ab.GetNR(id)

		if iter.GetValue() == "{...}"
		{
			return -2
		}
		iter = iter.Up
	}
	return -1
}
GetAlloc := !(Object^ Start,Type^ t) -> int
{
	iter := Start
	while iter != null
	{
		ab := iter.GetABox()
		if ab != null return ab.GetAlloc(t)
		if iter.GetValue() == "{...}"
		{
			return -2
		}
		iter = iter.Up
	}
	return -1
}
AllocSetStruct := !(Object^ start) -> void
{
	while start != null
	{
		aB := start.GetABox()
		if aB != null 
			aB.mustBeStruct = true
		start = start.Up
	}
}
