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

	if ob.IsConst return ob

	if ob is ObjIndent
	{
		asN := ob->{ObjIndent^}
		it := GetItem(asN.MyStr,ob)

		if it == null and asN.MyStr[0] == '$' 
		{
			inAtt := GlobalAttributes.TryFind(asN.MyStr[1..0])
			if inAtt != null return inAtt^
			return GBoolFalse //new ObjBool(false) 
		}
		if it == null return null
		if it is ObjConstHolder
			return it.Down
		if it is ConstItem
		{
			return it.Down
		}
		if it.IsConst return it
	}
	if ob.Down?.Right? is ObjSuffix
	{
		itItm := ob.Down
		asSuf := ob.Down.Right->{ObjSuffix^}
		if asSuf.MyStr == "f" and (itItm is ObjInt or itItm is ObjDouble)
		{
			newItm := new ObjDouble(0.0) ; $temp 
			newItm.ResultType = GTypeFloat
			if itItm isObjInt
			{
				newItm.MyDouble = itItm->{ObjInt^}.MyInt
			}else{
				newItm.MyDouble = itItm->{ObjDouble^}.MyDouble
			}
			return newItm
		}
	}
	//if ob.Down != null and ob.Down.IsConst and ob.Down.Right != null and ob.Down.Right.Right != null
	//and ob.Down.Right.Right.GetValue() == ".." and ob.Down.Right.Right.IsConst and Ob.Down.GetType
	//{
	//	
	//}
	//return null
	if ob is ObjData and ob.Down.GetValue() == "!" and ob.Down.Right.GetValue() == "[]"
	{
		SyntaxCompress(ob.Down.Right,PriorityData)
		iterR := ob.Down.Right.Down
		typ := Type^()
		Sutf := Queue.{Object^}() ; $temp
		
		while iterR != null
		{
			if iterR.GetValue() != ","
			{
				if iterR.Right != null and iterR.Right.GetValue() == ".."
				{
					if iterR.Right.Right != null and iterR.Right.Right.IsConst
						and iterR.GetType() == GTypeInt and iterR.Right.Right.GetType() == GTypeInt
					{
						start := iterR->{ObjInt^}.MyInt
						end := iterR.Right.Right->{ObjInt^}.MyInt

						if start > end return null

						for i : start..end Sutf.Push(new ObjInt(i)) ; $temp

						iterR = iterR.Right.Right

						if typ == null typ = GTypeInt
						typ = TypeFight(typ,GTypeInt)
						if typ == null return null
					}else{
						return null
					}
				}else{
					itm := TryCompute(iterR)
					if itm == null 
					{
						if iterR.GetType() == GTypeRange
						{
							if iterR.Down.GetType() != GTypeInt return null
							if iterR.Down.Right.GetType() != GTypeInt return null

							start := iterR.Down->{ObjInt^}.MyInt
							end := iterR.Down.Right.Right->{ObjInt^}.MyInt

							if start > end return null

							for i : start..end Sutf.Push(new ObjInt(i)) ; $temp

							if typ == null typ = GTypeInt
							typ = TypeFight(typ,GTypeInt)
							if typ == null return null
						}else{
							return null
						}
					}else{
						Sutf.Push(itm)
						if typ == null typ = itm.GetType()
						typ = TypeFight(typ,itm.GetType())
						if typ == null return null
					}
				}
			}
			iterR = iterR.Right
		}
		if typ == null or Sutf.Size() == 0 return null
		if typ == GTypeRange typ = GTypeInt
		return new ObjArray(typ.GetArray(Sutf.Size()),Sutf.ToArray()) ; $temp
	}

	return null
}
