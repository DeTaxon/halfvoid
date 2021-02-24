ExcPointers := AVLMap.{ Type^,AVLMap.{Type^, BoxFunc^} }

GetExcPointers := !(Type^ from, Type^ to) -> BoxFunc^
{
	if ExcPointers[from][to] != null
	{	
		assert(ExcPointers[from][to].MyFuncType.Pars[0] == from)
		return ExcPointers[from][to]
	}

	//if from is TypeArr and to is TypePoint
	//{
	//	if from.Base == to.Base
	//	{
	//		toAdd := new BuiltIn2ArrToPtr(from,to)	
	//		ExcPointers[from][to] = toAdd
	//		return toAdd
	//	}
	//}
	
	if from is TypeArr and to == GTypeVoidP
	{
		//toAdd := new BuiltInFuncUno("->{}",from,true,to,"#0 = bitcast "sbt + from.GetName() + "* #1 to " + to.GetName()+"\n")
		toAdd := new BuiltIn2PtrExc(from,to)
		ExcPointers[from][to] = toAdd
		return toAdd
	}
	//toAdd := new BuiltInFuncUno("->{}",from,false,to,"#0 = bitcast "sbt + from.GetName() + " #1 to " + to.GetName()+"\n")
	toAdd := new BuiltIn2PtrExc(from,to)
	ExcPointers[from][to] = toAdd
	
	return toAdd
}

ExcArrToPtr := class extend Object
{
	itId := int
	toType := Type^
	this := !(Type^ newType) -> void
	{
		itId = GetNewId()
		toType = newType
	}
	PrintPre := virtual !(sfile f) -> void
	{
		Down.PrintPointPre(f)
		f << "%T" << itId << " = getelementptr "
		Down.GetType().PrintType(f)
		f << " , "
		Down.PrintPointUse(f)
		f << ",i32 0,i32 0\n"
	}
	GetType := virtual !() -> Type^ { return toType }
	PrintUse := virtual !(sfile f) -> void
	{
		toType.PrintType(f)
		f << " %T" << itId
	}
	GetName := virtual !() -> char^
	{
		return "%T"sbt + itId
	}
	DoJIT := virtual !() -> void^ {
		return Down.DoJIT()
	}
	GetValue := virtual !() -> char^
	{
		return "x[]->x^"
	}
}
BuiltIn2PtrExc := class extend BuiltIn2Func
{
	this := !(Type^ frType, Type^ newType) -> void
	{
		FuncName = "->{}"
		MyFuncType = GetFuncType(![frType],newType)
	}
	PrintFunc := virtual !(BuiltIn2Call^ trg,sfile f) -> void {
		if trg.Down.GetType() is TypeArr
		{
			trg.GenId()
			trg.Down.PrintPointPre(f)
			trg.PrintRes(f)
			f << " = bitcast "
			trg.Down.PrintPointUse(f)
			f << " to "
			f << MyFuncType.RetType.GetName()
			f << "\n"
		}else{
			trg.GenId()
			trg.Down.PrintPre(f)
			trg.PrintRes(f)
			f << " = bitcast "
			trg.Down.PrintUse(f)
			f << " to "
			f << MyFuncType.RetType.GetName()
			f << "\n"
		}
	}
	DoJIT := virtual !(BuiltIn2Call^ trg) -> void^ {
		assert(false)
	}
}

PtrExchanger := class extend Object
{
	itId := int
	toType := Type^
	this := !(Type^ nwType) -> void
	{
		toType = nwType
		itId = GetNewId()
	}
	GetType := virtual !() -> Type^
	{
		return toType
	}

	IsRef := virtual !() -> bool { return false }

	PrintInBlock := virtual !(sfile f) -> void
	{
		PrintPre(f)
	}
	PrintPre := virtual !(sfile f) -> void
	{
		if Down.GetType() is TypeArr
		{
			Down.PrintPointPre(f)
			f << "%T" << itId << " = bitcast "
			Down.PrintPointUse(f)
			f << " to " << toType.GetName() << "\n"
		}else{
			Down.PrintPre(f)
			f << "%T" << itId << " = bitcast "
			Down.PrintUse(f)
			f << " to " << toType.GetName() << "\n"
		}
	}
	PrintUse := virtual !(sfile f) -> void
	{
		toType.PrintType(f)
		f << " %T" << itId
	}
	GetName := virtual !() -> char^
	{
		return "%T"sbt + itId
	}
}
