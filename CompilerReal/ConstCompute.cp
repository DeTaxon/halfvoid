

ConstItem := class extend Object
{
	Name := string
	this := !(string name, Object^ toCp) -> void
	{
		Name = name
		Down = toCp.Clone()
		Down.Up = this&
	}
	GetValue := virtual !() -> string
	{
		return "(const)"
	}
}

TryCompute := !(Object^ ob) -> Object^
{
	if ob == null return null

	if ob.IsConst() return ob

	if ob.GetValue() == "~ind"
	{
		asN := ob->{ObjIndent^}
		it := GetItem(asN.MyStr,ob)

		if it == null return null
		if it.GetValue() == "(const)"
		{
			return it.Down
		}
	}
	return null
}
