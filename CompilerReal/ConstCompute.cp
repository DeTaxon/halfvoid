

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
	lazy := ob.GetValue() == "~d"
	if lazy lazy = ob.Down.GetValue() == "!"
	if lazy lazy = ob.Down.Right.GetValue() == "[]"
	if lazy
	{
		
		iterR := ob.Down.Right.Down
		typ := ParseType(iterR)
		Sutf := Queue.{Object^}()
		
		if typ != null
		{
			iterR = iterR.Right
			if iterR.GetValue() == "," iterR = iterR.Right
		}
		while iterR != null
		{
			if iterR.GetValue() != ","
			{
				itm := TryCompute(iterR)
				if itm == null return null
				Sutf.Push(itm)
			}
			iterR = iterR.Right
		}
		if typ == null and Sutf.Size() == 0 return null
		typ = Sutf[0].GetType()
		return new ObjArray(typ.GetArray(Sutf.Size()),Sutf.ToArray())
	}

	return null
}
