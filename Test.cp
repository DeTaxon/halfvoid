printf := !(char^ str, ...) -> int declare
calloc := !(int size) -> void^ declare
free := !(void^ point) -> void declare

"deg" := !(double x) -> int
{
	return 0
}

main := !(int argc, char^^ argv) -> int
{
	j := 3.0deg
	printf("j = %f\n",j)
	return 0
}
