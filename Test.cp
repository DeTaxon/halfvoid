printf := !(char^ str, ...) -> int declare
memcpy := !(void^ a, void^ b,int size) -> void declare

wow := !() -> int
{
	printf("lazy\n")
	return 0
}

main := !(int argc, char^^ argv) -> int
{
	j := bool
	j = wow() and wow() and wow()
	return 0
}
