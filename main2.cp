


A := class .{@A}
{
	x := A
	if $keep
		y := A
}


main := !(int argc, char^^ argv) -> int
{
	a := A.{int}
	b := A.{int} ; $keep
	b.y = 0
	return 0
}

