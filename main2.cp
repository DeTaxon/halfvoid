


A := class .{@A}
{
	x := A
	Pop132132 := !() .{} -> void
	{
	}
}


main := !(int argc, char^^ argv) -> int
{
	a := A.{int}
	a.Pop132132()
	return 0
}

