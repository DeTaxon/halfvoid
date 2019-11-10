GetExchange := !(Object^ item, Object^ start, Type^ ToType,bool isRef) -> BoxFunc^
{
	itemType := item.GetType()
	SomeBugEnd := ToType

	if (itemType is TypePoint or itemType is TypeFatArr) 
	and (ToType is TypePoint or ToType is TypeFatArr)
	{
		return GetExcPointers(itemType,ToType)
	}

	for BuiltInExcs
	{
		ItType := it.MyFuncType
		if ItType.Pars[0] == item.GetType() and ItType.RetType == ToType
		{
			return it
		}
	}

	if isRef and ToType is TypeClass and itemType is TypeClass
	{
		box := new FuncInputBox() ; $temp 
		box.itPars.Emplace(itemType,isRef)
		box.itConsts.Push(new ObjType(ToType)) ; $temp
		preRet :=  GlobalRefExc^.GetFunc(box^)
		return preRet
	}

	if itemType is TypeArr and ToType is TypePoint
	{
		if itemType.Base == ToType.Base
		{
			box := new FuncInputBox() ; $temp
			box.itPars.Emplace(itemType,isRef)
			return GlobalExcArr^.GetFunc(box^)
		}
		if ToType == GTypeVoidP
		{
			return GetExcPointers(itemType,ToType)
		}
	}


	b := new FuncInputBox() ; $temp 

	b.itPars.Emplace(itemType,isRef)
	b.itConsts.Push(new ObjType(ToType)) ; $temp

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

ExcPointers := AVLMap.{ Type^,AVLMap.{Type^, BoxFunc^} }

GetExcPointers := !(Type^ from, Type^ to) -> BoxFunc^
{
	if ExcPointers[from][to] != null
	{
		return ExcPointers[from][to]
	}
	
	if from is TypeArr and to == GTypeVoidP
	{
		toAdd := new BuiltInFuncUno("->{}",from,true,to,"#0 = bitcast "sbt + from.GetName() + "* #1 to " + to.GetName()+"\n")
		ExcPointers[from][to] = toAdd
		return toAdd
	}
	toAdd := new BuiltInFuncUno("->{}",from,false,to,"#0 = bitcast "sbt + from.GetName() + " #1 to " + to.GetName()+"\n")
	ExcPointers[from][to] = toAdd
	return toAdd
}

BoxExc := !(Object^ item, Type^ toType, bool isRef) -> Object^
{
	if item.GetType() == null return null
	if item is SLambda
	{
		asN := item->{SLambda^}
		asN.ApplyFunc(toType,toType is TypePoint)
		return item		
	}
	if toType is TypeFuncLambda and item.GetType() is TypeFuncLambda
	{
		asN := item->{SLambda^}
		asN.ApplyFunc(toType,false)
		return item		
	}
	if toType is TypePoint and toType.Base is TypeFunc  and item.GetType() is TypeFuncLambda
	{
		asN := item->{SLambda^}
		asN.ApplyFunc(toType,true)
		return	item
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
