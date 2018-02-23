GetExchange := !(Object^ item, Object^ start, Type^ ToType) -> BoxFunc^
{
	//iter := start
	//TODO: Find "->{}"

	VT := GetType("void")
	VPT := VT.GetPoint()
	itemType := item.GetType()
	SomeBugEnd := ToType

	printf("here %s %s\n", itemType.GetType(), ToType.GetType())
	if itemType.GetType() == "point" and ToType.GetType() == "point"
	{
		return GetExcPointers(itemType,ToType)
	}

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

DoubleType := type pair.{Type^,Type^}

ExcPointers := Map.{DoubleType,BoxFunc^}

GetExcPointers := !(Type^ from, Type^ to) -> BoxFunc^
{
	//AsPair := pair.{Type^,Type^}
	//if ExcPointers.Exist(AsPair)
	//	return ExcPointers[AsPair]

	toAdd := new BuiltInFuncUno("->{}",from,false,to,"#0 = bitcast " + from.GetName() + " #1 to " + to.GetName()+"\n")
	//ExcPointers[AsPair] = toAdd
	return toAdd
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
