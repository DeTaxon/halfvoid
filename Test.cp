printf := !(char^ str, ...) -> int declare

wow := !() -> int
{
	printf("lazy\n")
	return 0
}

main := !(int argc, char^^ argv) -> int
{
	j := bool
	j = wow() or wow() or wow()
	return 0
}
