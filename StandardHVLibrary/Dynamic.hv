
internalCheckClassInClass := !(@FromType^ ptr).{@ToType} -> bool
{
	if ptr == null
		return false
	fTbl := FromType->VTable
	toTable := ToType->VTable
	tblPtr := ptr->{void^^}^
	tTable := tblPtr->{size_t^^}^
	if tTable == null
		return false
	for i : tTable^
	{
		if tTable->{void^^}[i*2 + 1] == toTable
			return true
	}
	return false
}