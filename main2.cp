
CC := class
{
	x := int
	this := !(int a) -> void
	{
		x = a
	}
}

main := !(int argc, char^^ argv) -> int
{

	c := new CC(15) ; $temp
	FlushTempMemory()
	h := new CC(12) ; $temp
	h.x = 8
	return 0
}

