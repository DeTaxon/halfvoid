GetExchange := !(Object^ item, Object^ start, Type^ ToType,bool isRef) -> BoxFunc^
{

	VT := GetType("void")
	VPT := VT.GetPoint()
	itemType := item.GetType()
	SomeBugEnd := ToType

	if (itemType.GetType() == "point" or itemType.GetType() == "fatarr") and ToType.GetType() == "point"
	{
		return GetExcPointers(itemType,ToType)
	}
	if itemType.GetType() == "fatarr" and ToType.GetType() == "fatarr"
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

	func :=  FindFunc("->{}",start,p,c,true)
	if func != null return func

	if itemType.GetType() == "class"
	{
		asNeed := (itemType->{TypeClass^}).ToClass
		func = asNeed.GetFunc("->{}",p,c)
		return func
	}
	return null
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
	if toType.GetType() == "lambda" and item.GetType().GetType() == "lambda"
	{
		asN := item->{SLambda^}
		asN.ApplyFunc(toType)
		return item		
	}
	Exc := GetExchange(item,item,toType,isRef)

	if Exc == null return null

	oldUp := item.Up
	oldRight := item.Right
	oldLeft := item.Left

	item.Up = null
	item.Right = null
	item.Left = null
	Call := MakeSimpleCall(Exc,item)

	Call.Left = oldLeft
	Call.Up = oldUp
	if oldLeft != null 
		oldLeft.Right = Call
	else
		oldUp.Down = Call
	Call.Right = oldRight
	if oldRight != null oldRight.Left = Call
	Call.Up = oldUp
	item.Up = Call

	return Call
}
