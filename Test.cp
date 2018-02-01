printf := !(char^ str, ...) -> int declare

wow := !() -> int
{
	printf("lazy\n")
	return 0
}

main := !(int argc, char^^ argv) -> int
{
	j := bool
	j = wow() == 0 or wow() == 0
	return 0
}
