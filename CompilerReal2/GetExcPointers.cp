ExcPointers := AVLMap.{ Type^,AVLMap.{Type^, BoxFunc^} }

GetExcPointers := !(Type^ from, Type^ to) -> BoxFunc^
{
	if ExcPointers[from][to] != null
	{	
		assert(ExcPointers[from][to].MyFuncType.Pars[0] == from)
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
		Down.PrintPre(f)
		f << "%T" << itId << " = bitcast "
		Down.PrintUse(f)
		f << " to " << toType.GetName() << "\n"
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
