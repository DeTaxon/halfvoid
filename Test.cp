printf := !(char^ str, ...) -> int declare
calloc := !(int size) -> void^ declare
free := !(void^ point) -> void declare

wut := !() -> double
{
	return 0.7
}


main := !(int argc, char^^ argv) -> int
{
	j := 3.0 - wut()
	printf("j = %f\n",j)
	return 0
}
