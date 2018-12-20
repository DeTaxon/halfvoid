#import "Tree.cp"

GetExchange := !(Object^ item, Object^ start, Type^ ToType,bool isRef) -> BoxFunc^
{

	VT := GetType("void")
	VPT := VT.GetPoint()
	itemType := item.GetType()
	SomeBugEnd := ToType

	if (itemType is TypePoint or itemType is TypeFatArr) 
	and (ToType is TypePoint or ToType is TypeFatArr)
	{
		return GetExcPointers(itemType,ToType)
	}
	//if itemType is TypeFatArr and ToType is TypeFatArr
	//{
	//	return GetExcPointers(itemType,ToType)
	//}

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

	if isRef and ToType is TypeClass and itemType is TypeClass
	{
		box := new FuncInputBox()
		box.itPars.Emplace(itemType,isRef)
		box.itConsts.Push(new ObjType(ToType))
		preRet :=  GlobalRefExc^.GetFunc(box^)
		return preRet
	}

	if itemType is TypeArr and ToType is TypePoint
	{
		if itemType.Base == ToType.Base
		{
			box := new FuncInputBox()
			box.itPars.Emplace(itemType,isRef)
			return GlobalExcArr^.GetFunc(box^)
		}
	}


	b := new FuncInputBox() 

	b.itPars.Emplace(itemType,isRef)
	b.itConsts.Push(new ObjType(ToType))

	func :=  FindFunc("->{}",start,b^,true)
	if func != null return func

	if itemType is TypeClass
	{
		asNeed := (itemType->{TypeClass^}).ToClass
		func = asNeed.GetFunc("->{}",b^,false)
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

	toAdd := new BuiltInFuncUno("->{}",from,false,to,"#0 = bitcast "sbt + from.GetName() + " #1 to " + to.GetName()+"\n")
	ExcPointers[from][to] = toAdd
	return toAdd
}

BoxExc := !(Object^ item, Type^ toType, bool isRef) -> Object^
{
	if item.GetType() == null return null
	if toType is TypeFuncLambda and item.GetType() is TypeFuncLambda
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
