
A := class
{
	x := int
	SetX := !(int f) -> void
	{
		x = f
	}
}

B := class extend A
{
}

C := class extend B
{
	Wow := !(int u) -> void
	{
		SetX(u)
	}
}
main := !(int argc, char^^ argv) -> int
{
	R := C
	R.Wow(4)
	return 0
}


