GetExchange := !(Object^ item, Object^ start, Type^ ToType) -> BoxFunc^
{
	//iter := start
	//TODO: Find "->{}"

	iterB := BuiltInFuncs.Start
	while iterB != null
	{
		if iterB.Data.FuncName == "->{}"
		{
			ItType := iterB.Data.MyFuncType
			if ItType.Pars[0] == item.GetType() and ItType.RetType == ToType
			{
				return iterB.Data
			}
		}
		iterB = iterB.Next
	}
	return null
}

BoxExc := !(Object^ item, Type^ toType) -> bool
{
	Exc := GetExchange(item,item,toType)
	
	if Exc == null return false

	Call := MakeSimpleCall(Exc,item)
	ReplaceNode(item,Call)
	Call.Down = item
	item.SetUp(Call)

	return true
}
