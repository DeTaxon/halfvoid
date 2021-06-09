
internalCheckClassInClass := !(@FromType^ ptr).{@ToType} -> bool
{
	fTbl := FromType->VTable
	toTable := ToType->VTable
	tTable := fTbl->{size_t^^}^
	if tTable == null
		return false
	for i : tTable^
	{
		if tTable->{void^^}[i*2 + 1] == toTable
			return true
	}
	return false
}