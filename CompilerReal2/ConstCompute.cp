// TODO: это костыль, нужно заменить на оптимизатор и более умный WorkBag
#import "Tree.cp"
#import "Chairwheel.cp"

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

	if ob.GetValue() == "~ind"
	{
		asN := ob->{ObjIndent^}
		it := GetItem(asN.MyStr,ob)

		if it == null return null
		if it.GetValue() == "(const)"
		{
			return it.Down
		}
		if it.IsConst return it
	}
	if ob.GetValue() == "~str" and ob.Right != null
		and ob.Right.GetValue() == "~suffix"
	{
		asStr := ob->{ObjStr^}
		asSuf := ob.Right->{ObjSuffix^}
		val := TryCheckSuffix(asStr.GetString(),asSuf.MyStr)
		if val != null return val
	}
	lazy := ob.GetValue() == "~d"
	if lazy lazy = ob.Down.GetValue() == "!"
	if lazy lazy = ob.Down.Right.GetValue() == "[]"
	if lazy and false
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
				if itm == null 
				{
					return null
					if iterR.GetType() == TypeTable[13]
					{
						if iterR.Down.GetType() != GetType("int") return null
						if iterR.Down.Right.GetType() != GetType("int") return null
						return null

						start := iterR.Down->{ObjInt^}.MyInt
						end := iterR.Down.Right.Right->{ObjInt^}.MyInt

						if start > end return null

						for i : start..end Sutf.Push(new ObjInt(i))
					}else{
						return null
					}
				}
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
