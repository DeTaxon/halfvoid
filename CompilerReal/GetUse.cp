InspectDataR := !(Object^ item) -> Object^
{
	

	return null
}

GetUse := !(Object^ item) -> Object^
{
	GotObject := null->{Object^}

	if (item->GetValue() == "~d")
	{
		res := InspectDataR(item)
		if res != null return res
	}

	return GotObject
}
