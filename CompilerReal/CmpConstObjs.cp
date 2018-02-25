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
	if a.GetValue() == "~type" and b.GetValue() == "~type"
	{
		aT := a->{ObjType^}
		bT := b->{ObjType^}
		return aT.MyType == bT.MyType
	}
	// float, func ??
	return false
}
