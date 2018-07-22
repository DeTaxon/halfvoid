

AllocBox := class 
{
	ItemBag := Map.{int,Type^}
	GetAlloc := !(Type^ ToAdd) -> int
	{
		DaID := GetNewId()
		ItemBag[DaID] = ToAdd
		return DaID
	}
	//ReturnAlloc := !()
	//PrintGlobal := !(sfile f) -> void
	//{
	//}
	PrintAlloc := !(sfile f) -> void
	{
		iter := ItemBag.Start
		while iter != null
		{
			f << "%T" << iter.Key <<" = alloca "
			iter.Value.PrintType(f)
			f << "\n"

			iter = iter.Next
		}
	}
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
