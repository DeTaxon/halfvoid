
A := class
{
	x := int
	SetX := !(int f) -> void
	{
		x = f
	}
	Waw := !() -> void
	{
		SetX(3)
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
	R.Wow(5)
	return 0
}


