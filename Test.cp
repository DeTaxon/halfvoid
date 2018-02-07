printf := !(char^ str, ...) -> int declare
calloc := !(int size) -> void^ declare
free := !(void^ point) -> void declare

"deg" := !(int x) -> int
{
	return 0
}

main := !(int argc, char^^ argv) -> int
{
	j := 7deg
	printf("j = %f\n",j)
	return 0
}
