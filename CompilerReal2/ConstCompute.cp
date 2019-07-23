// TODO: это костыль, нужно заменить на оптимизатор и более умный WorkBag
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
			return new ObjBool(false) //GBoolFalse
		}
		if it == null return null
		if it is ConstItem
		{
			return it.Down
		}
		if it.IsConst return it
	}
	if ob is ObjStr and ob.Right != null
		and ob.Right is ObjSuffix
	{
		asStr := ob->{ObjStr^}
		asSuf := ob.Right->{ObjSuffix^}
		val := TryCheckSuffix(asStr.GetString(),asSuf.MyStr)
		if val != null return val
	}
	//if ob.Down != null and ob.Down.IsConst and ob.Down.Right != null and ob.Down.Right.Right != null
	//and ob.Down.Right.Right.GetValue() == ".." and ob.Down.Right.Right.IsConst and Ob.Down.GetType
	//{
	//	
	//}

	lazy := ob  is ObjData
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
				if iterR.Right != null and iterR.Right.GetValue() == ".."
				{
					if iterR.Right.Right != null and iterR.Right.Right.IsConst
						and iterR.GetType() == GTypeInt and iterR.Right.Right.GetType() == GTypeInt
					{
						start := iterR->{ObjInt^}.MyInt
						end := iterR.Right.Right->{ObjInt^}.MyInt

						if start > end return null

						for i : start..end Sutf.Push(new ObjInt(i))

						iterR = iterR.Right.Right
						
					}else{
						return null
					}
				}else{
					itm := TryCompute(iterR)
					if itm == null 
					{
						//return null
						if iterR.GetType() == GTypeRange
						{
							if iterR.Down.GetType() != GTypeInt return null
							if iterR.Down.Right.GetType() != GTypeInt return null
							//return null

							start := iterR.Down->{ObjInt^}.MyInt
							end := iterR.Down.Right.Right->{ObjInt^}.MyInt

							if start > end return null

							for i : start..end Sutf.Push(new ObjInt(i))
						}else{
							return null
						}
					}else{
						Sutf.Push(itm)
					}
				}
			}
			iterR = iterR.Right
		}
		if typ == null and Sutf.Size() == 0 return null
		typ = Sutf[0].GetType()
		if typ == GTypeRange typ = GTypeInt
		return new ObjArray(typ.GetArray(Sutf.Size()),Sutf.ToArray())
	}

	return null
}
