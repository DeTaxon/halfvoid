
Type := class {
	Id := int
	Base := Type^
	ItPoint := Type^
	
	this := !() -> void
	{
		Clean()
	}
	Clean := !() -> void
	{
		Base = null
		ItPoint = null
	}
	GetType := !() -> string
	{
		return ""
	}

	GetPoint := !() -> Type^
	{
		if ItPoint == null{
			ItPoint = new TypePoint(this&)
		}
		return ItPoint
	}

}

ParseType := !(Object^ Node) -> Type^
{
	if Node == null return null

	if Node.GetValue() == "~ind"
	{
		NodeName := GetItem(Node->{ObjIndent^}.MyStr,Node)
		if NodeName == null return null
	}
	return null
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
}

TypeTable := Type^[12]
DefsTable := TypeDef^[15]

CreateStandart := !() -> void
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
