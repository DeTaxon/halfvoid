InspectDataR := !(Object^ item) -> Object^
{
	DaR := null->{Object^}
	DaR = ParseFuncDataR(item)
	if DaR != null return DaR

	DaR = GetFuncCall(item)
	if DaR != null return DaR

	DaR = ParseParamCall(item)
	if DaR != null return DaR
	
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
