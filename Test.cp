printf := !(char^ str, ...) -> int declare
calloc := !(int size) -> void^ declare
free := !(void^ point) -> void declare

"deg" := !(int x) -> double
{
	return x*3.14/180.0
}

main := !(int argc, char^^ argv) -> int
{
	j := 2deg
	printf("j = %i\n",argc)
	return 0
}
