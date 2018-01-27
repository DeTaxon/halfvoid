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
