printf := !(char^ a,...) -> void declare

main := !() -> int
{
	x := 3
	defer printf("a\n")
	printf("b\n")
	return 0
}

