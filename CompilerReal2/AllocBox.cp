#import "Tree.cp"

AllocBox := class 
{
	ItId := int

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
	//ReturnAlloc := !()
	PrintGlobal := !(sfile f) -> void
	{
		if not ItemBag.Empty()
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


			f << "}\n"
		}
	}
	GetAsUse := !() -> string
	{
		return "%AllocClass" + ItId + "* %AllocItem" + ItId
	}
	PrintAlloc := !(sfile f) -> void
	{
		if not ItemBag.Empty()
		{
			f << "%AllocItem" << ItId << " = alloca %AllocClass" << ItId << "\n"
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
		//if iter.GetValue() == "x=>x"
		//{
		//	asL2 := iter->{SLambda^}
		//	return asL2.ABox.GetNR(id)
		//}
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
		//if iter.GetValue() == "x=>x"
		//{
		//	asL2 := iter->{SLambda^}
		//	return asL2.ABox.GetAlloc(t)
		//}
		if iter.GetValue() == "{...}"
		{
			return -2
		}
		iter = iter.Up
	}
	return -1
}
