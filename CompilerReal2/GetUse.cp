#import "Tree.cp"
#import "OMPStyle.cp"

InspectDataR := !(Object^ item) -> Object^
{
	DaR := null->{Object^}
	DaR = ParseFuncDataR(item)
	if DaR != null->{int^} return DaR	


	DaR = GetFuncCall(item)
	if DaR != null->{int^} return DaR

	DaR = ParseParamCall(item)
	if DaR != null->{int^} return DaR

	if item.GetValue() == "()" return item.Down
	
	return null
}

GetUse := !(Object^ item) -> Object^
{
	GotObject := null->{Object^}

	//if item.GetValue() == "~d" or item.GetValue() == "~ind"
	if item is ObjData or item is ObjIndent 
	{
		res := InspectDataR(item)
		if res != null return res
	}

	return GotObject
}
