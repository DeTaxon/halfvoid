printf := !(char^ str, ...) -> int declare

wow := !() -> int
{
	printf("lazy\n")
	return 0
}

main := !(int argc, char^^ argv) -> int
{
	j := bool
	if wow() or wow() 
		printf("hello\n")
	if wow()printf("hello\n")
	return 0
}
