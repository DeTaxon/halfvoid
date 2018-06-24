
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
		//later
	}
	resB[0] = false
	return null
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
				if iter.GetValue() != "~{}type"
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
