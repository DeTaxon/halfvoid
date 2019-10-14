
AllocBox := class 
{
	ItId := int
	liveOnGlobal := bool

	parentAlloc := AllocBox^
	inhAllocs := List.{AllocBox^}


	ItemBag := Map.{int,Type^}
	GetAlloc := !(Type^ ToAdd) -> int
	{
		DaID := GetNewId()
		ItemBag[DaID] = ToAdd
		return DaID
	}
	GetClassName := !() -> string
	{
		return "%AllocClass" + ItId
	}
	GetNR := !(int id) -> int
	{
		i := 0
		iter := ItemBag.Start
		while iter != null
		{
			if iter.Key == id return i
			iter = iter.Next
			i += 1
		}
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
			if not it.ItemBag.Empty()
				cntr += 1
		}
		return cntr
	}
	//ReturnAlloc := !()

	printedGlobal := bool
	PrintGlobal := !(sfile f) -> void
	{
		if printedGlobal 
			return void
		printedGlobal = true 

		inhP := false
		inhP = inhP or  not inhAllocs[^].ItemBag.Empty()
		if (not ItemBag.Empty()) or inhP
		{
			f << "%AllocClass" << ItId << " = type {"
			
			sIter := ItemBag.Start
			i := 0

			while sIter != null
			{	
				if i != 0 f << " , "
				f << sIter.Value.GetName()
				sIter = sIter.Next
				i += 1
			}
			if not inhAllocs[^].ItemBag.Empty()
			{
				if i != 0 f << " , "
				f << it.GetClassName()
				i += 1
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
		if ItemBag.Empty()
			return "%AllocClass"sbt + ItId + "* null"
		return "%AllocClass"sbt + ItId + "* %AllocItem" + ItId
	}
	PrintAlloc := !(sfile f) -> void
	{
		PrintAlloc(f,null->{char^})
	}
	PrintAlloc := !(sfile f,char^ prntName) -> void
	{
		if not ItemBag.Empty()
		{
			if liveOnGlobal
			{
				f << "%AllocItem" << ItId << " = getelementptr %AllocClass" << ItId << " , %AllocClass"
					<< ItId << "* @AllocObject" << ItId << ", i32 0\n"
			}else{
				if parentAlloc != null
				{
					assert(prntName != null)
					cntr := InheritNR()
					f << "%AllocItem" << ItId << " = getelementptr %AllocClass" << parentAlloc.ItId << " , "
						f << " %AllocClass" << parentAlloc.ItId << "* " << prntName
						f << ", i32 0, i32 " << cntr << "\n"
				}else{
					f << "%AllocItem" << ItId << " = alloca %AllocClass" << ItId << "\n"
				}
			}
		}

		PrintBoxItems(f,"%AllocItem" + ItId)
	}
	PrintBoxItems := !(sfile f,string Name) -> void
	{
		iter := ItemBag.Start
		i := 0
		while iter != null
		{
			f << "%T" << iter.Key <<" = getelementptr %AllocClass" << ItId << " , %AllocClass" << ItId << "* " + Name
			f << " , i32 0, i32 " << i<<"\n"

			iter = iter.Next
			i += 1
		}
	}
}

GetAllocNR := !(Object^ Start,int id) -> int
{
	iter := Start
	while iter != null
	{
		if iter.GetValue() == "!()"
		{
			dynCast := iter->{BoxFunc^}
			return dynCast.ABox.GetNR(id)
		}
		if iter.GetValue() == "x=>x"
		{
			asL2 := iter->{SLambda^}
			return asL2.ABox.GetNR(id)
		}
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
		if iter.GetValue() == "!()"
		{
			dynCast := iter->{BoxFunc^}
			return dynCast.ABox.GetAlloc(t)
		}
		if iter.GetValue() == "x=>x"
		{
			asL2 := iter->{SLambda^}
			return asL2.ABox.GetAlloc(t)
		}
		if iter.GetValue() == "{...}"
		{
			return -2
		}
		iter = iter.Up
	}
	return -1
}
