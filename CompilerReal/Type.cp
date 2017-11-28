
TypeTable := Queue.{Type^}

Type := class {
	Id := int
	Base := Type^
	ItType := int
	
	this := !() -> void
	{
		Clean()
	}
	Clean := !() -> void
	{
		Base = null
		ItType = 0
	}
	GetType := !() -> string
	{
		return ""
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

CreateStandart := !() -> void
{
	TypeTable.Push(new TypeStandart("i8"))	
	TypeTable.Push(new TypeStandart("i16"))	
	TypeTable.Push(new TypeStandart("i32"))	
	TypeTable.Push(new TypeStandart("i64"))

	TypeTable.Push(new TypeStandart("i8"))	
	TypeTable.Push(new TypeStandart("i16"))	
	TypeTable.Push(new TypeStandart("i32"))	
	TypeTable.Push(new TypeStandart("i64"))	
}
