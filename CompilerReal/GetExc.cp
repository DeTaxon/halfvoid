GetExchange := !(Object^ item, Object^ start, Type^ ToType,bool isRef) -> BoxFunc^
{

	VT := GetType("void")
	VPT := VT.GetPoint()
	itemType := item.GetType()
	SomeBugEnd := ToType

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

	if isRef and ToType.GetType() == "class" and itemType.GetType() == "class"
	{
		pars := Queue.{Type^}()
		consts := Queue.{Object^}()
		pars.Push(itemType)
		consts.Push((new ObjType(ToType))->{Object^})
		preRet :=  GlobalRefExc^.GetFunc(pars,consts)
		return preRet
	}

	if itemType.GetType() == "arr" and ToType.GetType() == "point"
	{
		if itemType.Base == ToType.Base
		{
			pars := Queue.{Type^}()
			pars.Push(itemType)
			return GlobalExcArr^.GetFunc(pars)
		}
	}

	p := Queue.{Type^}()
	c := Queue.{Object^}()

	p.Push(itemType)
	c.Push(new ObjType(ToType))

	return FindFunc("->{}",start,p,c,false)
}

ExcPointers := Map.{ Type^,Map.{Type^, BoxFunc^} }

GetExcPointers := !(Type^ from, Type^ to) -> BoxFunc^
{
	if ExcPointers[from][to] != null
	{
		return ExcPointers[from][to]
	}

	toAdd := new BuiltInFuncUno("->{}",from,false,to,"#0 = bitcast " + from.GetName() + " #1 to " + to.GetName()+"\n")
	ExcPointers[from][to] = toAdd
	return toAdd
}

BoxExc := !(Object^ item, Type^ toType, bool isRef) -> Object^
{
	Exc := GetExchange(item,item,toType,isRef)
	
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
