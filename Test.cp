printf := !(char^ str, ...) -> int declare
calloc := !(int size,int sizet) -> void^ declare
malloc := !(int size) -> void^ declare
free := !(void^ point) -> void declare

ve := class
{
	x,y,z := int
}

main := !(int argc, char^^ argv) -> int
{
	waw := ve^
	waw = new ve
	waw.z = 5
	printf("wow %i\n",waw.z)
	free(waw)
	return 0
}
