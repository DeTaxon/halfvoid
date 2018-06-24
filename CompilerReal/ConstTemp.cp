
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

IsEqConsts := !(Object^ l, Queue.{Object^} consts, Queue.{ObjConstHolder^} res) -> bool
{
	return false
}
