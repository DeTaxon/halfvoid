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
		if itemType == VPT
		{
			return GetExcFromVoidP(SomeBugEnd)
		}else{
			return GetExcToVoidP(SomeBugEnd)
		}
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

ExcToVoid := Map.{Type^,BoxFunc^}
ExcFromVoid := Map.{Type^,BoxFunc^}

GetExcToVoidP := !(Type^ toCh) -> BoxFunc^
{
	if ExcToVoid.Exist(toCh)
		return ExcToVoid[toCh]
	
	VT := GetType("void")
	VPT := VT.GetPoint()

	toAdd := new BuiltInFuncUno("->{}",toCh,false,VPT,"#0 = bitcast " + toCh.GetName() + " #1 to i8*\n")
	ExcToVoid[toCh] = toAdd
	return toAdd
}
GetExcFromVoidP := !(Type^ toCh) -> BoxFunc^
{
	if ExcFromVoid.Exist(toCh)
		return ExcToVoid[toCh]
	
	VT := GetType("void")
	VPT := VT.GetPoint()

	toAdd := new BuiltInFuncUno("->{}",toCh,false,VPT,"#0 = bitcast i8* #1 to "+toCh.GetName()+"\n")
	ExcToVoid[toCh] = toAdd
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
