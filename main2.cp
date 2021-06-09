
TstCl := class
{
	c := virtual !() -> void
	{

	}
}
Cl2 := class extend TstCl
{
}
Cl3 := class extend Cl2
{
}

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
main := !(int argc, char^^ argv) -> int
{
	r := new Cl2
	if r is in TstCl
	{
		printf("yep\n")
	}else{
		printf("nope\n")
	}
}
