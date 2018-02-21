printf := !(char^ str, ...) -> int declare
calloc := !(int size) -> void^ declare
free := !(void^ point) -> void declare

main := !(int argc, char^^ argv) -> int
{
	test := int^
	test = calloc(4)
	test^ = 4
	printf("some %i\n",test^)
	return 0
}
