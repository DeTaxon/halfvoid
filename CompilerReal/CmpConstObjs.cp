CmpConstObjs := !(Object^ a, Object^ b) -> bool
{
	if a == null return false
	if b == null return false

	if a.GetValue() == "~int" and b.GetValue() == "~int"
	{
		aI := a->{ObjInt^}
		bI := b->{ObjInt^}
		return aI.MyInt == bI.MyInt
	}
	if a.GetValue() == "~str" and b.GetValue() == "~str"
	{
		aS := a->{ObjStr^}
		bS := b->{ObjStr^}
		return aS.MyStrId == bS.MyStrId
	}
	// float, func ??
	return false
}
