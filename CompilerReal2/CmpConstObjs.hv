CmpConstObjs := !(Object^ a, Object^ b) -> bool
{
	if a == null return false
	if b == null return false

//	if a.GetValue() != b.GetValue() return false

	if a is ObjType and b is ObjType
	{
		aT := a->{ObjType^}
		bT := b->{ObjType^}
		return aT.MyType == bT.MyType
	}
	if a is ObjInt and b is ObjInt
	{
		aI := a->{ObjInt^}
		bI := b->{ObjInt^}
		return aI.MyInt == bI.MyInt
	}
	if a is ObjStr and b is ObjStr
	{
		aS := a->{ObjStr^}
		bS := b->{ObjStr^}
		return aS.MyStrId == bS.MyStrId
	}
	if a is ObjBool and b is ObjBool
	{
		aS := a->{ObjBool^}
		bS := b->{ObjBool^}
		return aS.MyBool == bS.MyBool
	}
	// float, func ??
	return false
}
SpaceCmpConstObjs := !(Object^ a, Object^ b) -> int
{
	if a is ObjInt and b is ObjInt
	{
		aI := a->{ObjInt^}
		bI := b->{ObjInt^}
		return aI.MyInt <=> bI.MyInt
	}
	if a is ObjStr and b is ObjStr
	{
		aS := a->{ObjStr^}
		bS := b->{ObjStr^}
		return aS.GetString() <=> bS.GetString()
	}
	assert(false)
	return 0
}
