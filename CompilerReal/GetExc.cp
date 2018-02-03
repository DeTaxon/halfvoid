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

BoxExc := !(Object^ item, Type^ toType) -> Object^
{
	Exc := GetExchange(item,item,toType)
	
	if Exc == null return null

	oldUp := item.Up
	oldRight := item.Right
	oldLeft := item.Left

	item.Up = null
	item.Right = null
	item.Left = null
	Call := MakeSimpleCall(Exc,item)

	item.Up = oldUp
	item.Right = oldRight
	item.Left = oldLeft
	ReplaceNode(item,Call)
	item.SetUp(Call)


	return Call
}
