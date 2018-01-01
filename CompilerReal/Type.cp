
Type := class {
	Id := int
	Base := Type^
	AsPoint := Type^
	AsArray := Queue.{TypeArr^}
	
	this := !() -> void
	{
		Clean()
	}
	Clean := !() -> void
	{
		Base = null
		AsPoint = null
		AsArray.Start = null
	}
	GetType := !() -> string
	{
		return ""
	}

	GetPoint := !() -> Type^
	{
		if AsPoint == null{
			AsPoint = new TypePoint(this&)
		}
		return AsPoint
	}
	GetArray :=  !(int size) -> TypeArr^
	{
		iterE := AsArray.Start

		while iterE != null
		{
			if iterE.Data.Size == size return iterE.Data
			iterE = iterE.Next
		}

		NewArr := new TypeArr(this&,size)
		AsArray.Push(NewArr)
		return NewArr
	}

	PrintType := virtual !() -> void
	{
	}

}

ParseType := !(Object^ Node) -> Type^
{
	if Node == null return null

	if Node.GetValue() == "~ind"
	{
		NodeName := GetItem(Node->{ObjIndent^}.MyStr,Node)
		if NodeName == null return null
		//NodeName.GetValue()
		if NodeName.GetValue() == ":=type"
		{
			return NodeName->{TypeDef^}.ItType
		}
		return null
	}
	if Node.GetValue() == "~d"
	{
		if not InDataR(Node.Down) return null
		Ri := Node.Down.Right
		if Ri == null return null
		
		under := ParseType(Node.Down)
		if under == null return null

		if Ri.GetValue() == "^"
		{
			return under.GetPoint()
		}
		if Ri.GetValue() == "[]"
		{
			if Ri.Down.GetValue() == "~int"
			{
				Ri = Ri.Down
				DynCast := Ri->{ObjInt^}
				ArrSize := DynCast.MyInt
				return under.GetArray(ArrSize->{int})->{Type^}
			}
			return null
		}


	}
	return null
}

FuncTypeTable := Queue.{TypeFunc^}

GetFuncType := !(Queue.{Type^} lin, bool IsVArgs) -> TypeFunc^
{
	iterT := FuncTypeTable.Start

	while iterT != null
	{
		SomeBug := IsVArgs
		if iterT.Data.IsVArgs == SomeBug
		{
			IsFound := true
			if iterT.Data.ParsCount == lin.Size()
			{
				iterR := lin.Start
				i := 0
				while iterR != null
				{
					if iterT.Data.Pars[i] != iterR.Data IsFound = false
					i += 1
					iterR = iterR.Next 
				}
			}else{
				IsFound = false
			}
			if IsFound return iterT.Data
		}
		iterT = iterT.Next
	}
	

	newTypeFunc := new TypeFunc(lin,IsVArgs)
	FuncTypeTable.Push(newTypeFunc)
	return newTypeFunc
}

TypeDef := class extend Object
{
	ItName := string
	ItType := Type^

	this := !(string name, Type^ T) -> void
	{
		Clean()
		ItName = name
		ItType = T
	}
	GetValue := virtual !() -> string
	{
		return ":=type"
	}
}

TypeStandart := class extend Type{
	IRName := string
	
	this := !(string Name) -> void
	{
		IRName = Name 
		Clean()
	}
	GetType := !() -> string
	{
		return "standart"
	}
	PrintType := virtual !() -> void
	{
		printf(IRName)
	}
}

TypePoint := class extend Type
{
	this := !(Type^ nBase) -> void
	{
		Clean()
		Base = nBase
	}
	GetType := !() -> string
	{
		return "point"
	}
	PrintType := virtual !() -> void
	{
		Base.PrintType()
		printf("*")
	}
}

TypeFunc := class extend Type
{
	ParsCount := int
	Pars := Type^^
	IsVArgs := bool

	this := !(Queue.{Type^} P, bool IsV) -> void
	{
		ParsCount = P.Size()
		Pars = null
		if ParsCount != 0 Pars = P.ToArray()
		IsVArgs = IsV
	}
	GetType := !() -> string
	{
		return "function"
	}
}

TypeArr := class extend Type
{
	Size := int

	this := !(Type^ B, int S) -> void
	{
		Base = B
		Size = S
	}
	GetType := !() -> string
	{
		return "arr"
	}
	PrintType := virtual !() -> void
	{
		printf("[%i x ",Size)
		Base.PrintType()
		printf("]")
	}
}

TypeTable := Type^[30]
DefsTable := TypeDef^[15]

CreateStandartTypes := !() -> void
{	
	//u
	TypeTable[0] = new TypeStandart("i8")	
	TypeTable[1] = new TypeStandart("i16")	
	TypeTable[2] = new TypeStandart("i32")	
	TypeTable[3] = new TypeStandart("i64")

	//s
	TypeTable[4] = new TypeStandart("i8")	
	TypeTable[5] = new TypeStandart("i16")	
	TypeTable[6] = new TypeStandart("i32")	
	TypeTable[7] = new TypeStandart("i64")

	TypeTable[8] = new TypeStandart("i1")	// bool

	TypeTable[9] = new TypeStandart("float")
	TypeTable[10] = new TypeStandart("double")
	TypeTable[11] = new TypeStandart("void")

	DefsTable[0] = new TypeDef("u8",TypeTable[0])
	DefsTable[1] = new TypeDef("u16",TypeTable[1])
	DefsTable[2] = new TypeDef("u32",TypeTable[2])
	DefsTable[3] = new TypeDef("u64",TypeTable[3])

	DefsTable[4] = new TypeDef("s8",TypeTable[4])
	DefsTable[5] = new TypeDef("s16",TypeTable[5])
	DefsTable[6] = new TypeDef("s32",TypeTable[6])
	DefsTable[7] = new TypeDef("s64",TypeTable[7])

	DefsTable[8] = new TypeDef("bool",TypeTable[8])
	DefsTable[9] = new TypeDef("char",TypeTable[0])
	DefsTable[10] = new TypeDef("short",TypeTable[1])
	DefsTable[11] = new TypeDef("int",TypeTable[2])
	DefsTable[12] = new TypeDef("long",TypeTable[3])
	DefsTable[13] = new TypeDef("float",TypeTable[10])
	DefsTable[14] = new TypeDef("double",TypeTable[11])

}
