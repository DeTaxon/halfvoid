
IsSameType := !(Object^ obj,Type^ itT ,Queue.{ObjConstHolder^} res, bool^ resB) -> Type^
{
	if obj == null return null
	
	if obj.GetValue() == "~ind"
	{
		val := ParseType(obj)
		if val != itT resB[0] = false
		return val
	}
	if obj.GetValue() == "~{}type"
	{
		sNeed := obj->{ObjTemplateType^}
		res.Push(new ObjConstHolder(sNeed.MyStr,(new ObjType(itT))->{Object^}))
		return itT
	}
	if obj.GetValue() == "~d"
	{
		if obj.Down.Right == null return IsSameType(obj.Down,itT,res,resB)
		if obj.Down.Right.GetValue() == "^"
		{
			if itT.GetType() != "point"
			{
				resB[0] = false
				return null
			}
			preT :=  IsSameType(obj.Down,itT.Base,res,resB)
			if preT != null {
				return preT.GetPoint()
			}
			return null
		}
		if obj.Down.Right.GetValue() == "[]"
		{
			if itT.GetType() != "arr"{
				if itT.GetType() == "fatarr"
				{
					typD2 := IsSameType(obj.Down,itT.Base,res,resB)
					if typD2 == null return null
					
					if obj.Down.Right.Down != null 
					{
						resB[0] = false
						return null
					}
					return typD2.GetFatArray()
				}
				resB[0] = false
				return null
			}
			asNeedType := itT->{TypeArr^}

			typD := IsSameType(obj.Down,itT.Base,res,resB)
			if typD == null return null

			itemD := obj.Down.Right.Down
			if itemD == null return null
			
			if itemD.IsConst()
			{
				if itemD.GetValue() != "~int"
				{
					resB[0] = false
					return null
				}
				asNeedDown := itemD->{ObjInt^}
				if asNeedDown.MyInt != asNeedType.Size
				{
					resB[0] = false
					return null
				}
				return null				
			}else{
				if itemD.GetValue() == "~{}type"
				{
					asNeedDown2 := itemD->{ObjTemplateType^}
					res.Push(new ObjConstHolder(asNeedDown2.MyStr,(new ObjInt(asNeedType.Size))->{Object^}))
					return itT
				}else{
					resB[0] = false
					return null
				}
			}
		}
	}
	resB[0] = false
	return null
}

InHolder := class extend Object
{
	this := !(Object^ v) -> void
	{
		Down = v.Down
		Down.SetUp(this&)
		PopOutNode(Down.Right)
		v.Down = null
	}
	GetValue := virtual !() -> string
	{
		return "~{in}type"
	}
}

MakeGoodConsts := !(Object^ l) -> bool
{
	iter := l.Down

	while iter != null
	{
		if iter.GetValue() != ","
		{
			if not iter.IsConst() 
			{
				gotIt := false
				if iter.GetValue() == "~d"
				{
					if iter.Down.Right != null
					{
						if iter.Down.Right.GetValue() == "in"
						{
							gotIt = true
							cr := new InHolder(iter)
							iter  = ReplaceNode(iter,cr)
						}
					}
				}
				if iter.GetValue() != "~{}type" and not gotIt
				{
					val := ParseType(iter)

					if val != null
					{
						iter = ReplaceNode(iter,new ObjType(val))
					}// else error
				}
			}
		}
		iter = iter.Right
	}
	return true
}

IsEqConsts := !(Object^ l, Queue.{Object^} consts, Queue.{ObjConstHolder^} res) -> bool
{
	re := bool
	iter := l.Down

	i := 0
	while iter != null
	{
		if iter.GetValue() != ","
		{
			if not iter.IsConst()
			{
				if iter.GetValue() == "~{in}type"
				{
					if consts[i].GetValue() != "~type" return false
					asNeed := iter->{InHolder^}
					asN2 := consts[i]->{ObjType^}
					
					if asN2.MyType == null return false
					if asN2.MyType.GetType() != "class" return false

					val := ParseType(asNeed.Down.Right)
					if val.GetType() != "class" return false
					val2 := val->{TypeClass^}
					re = true
					val3 := IsSameType(asNeed.Down,asN2.MyType,res,re&)
					if not re return false

					iterInT := ((val3->{TypeClass^}).ToClass)
					found := false

					while iterInT != null
					{
						if iterInT.ClassType
							found  = true
						iterInT = iterInT.Parent
					}

					if not found return false

				}else{
					if consts[i].GetValue() == "~type"
					{
						asNeed := consts[i]->{ObjType^}
						re = true
						IsSameType(iter,asNeed.MyType,res,re&)
						if not re return false
					}else{
						if iter.GetValue() != "~{}type" return false
						asNeed := iter->{ObjTemplateType^}
						res.Push(new ObjConstHolder(asNeed.MyStr,consts[i]))
					}
				}
			}else{
				if not CmpConstObjs(iter,consts[i])
					return false
			}
			i += 1
		}
		iter = iter.Right
	}
	return true
}
