printf := !(char^ str, ...) -> int declare
calloc := !(int size) -> void^ declare
malloc := !(int size) -> void^ declare
free := !(void^ point) -> void declare

main := !(int argc, char^^ argv) -> int
{
	test := int^
	test = malloc(4)
	printf("point = %p\n",test)
	test^ = 4
	printf("some %i\n",test^)
	free(test)
	return 0
}
