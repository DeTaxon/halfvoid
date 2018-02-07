printf := !(char^ str, ...) -> int declare
calloc := !(int size) -> void^ declare
free := !(void^ point) -> void declare

main := !(int argc, char^^ argv) -> int
{
	j := 3.0f
	printf("j = %f\n",j)
	return 0
}
