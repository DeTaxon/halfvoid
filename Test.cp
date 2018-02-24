printf := !(char^ str, ...) -> int declare
calloc := !(int size) -> void^ declare
malloc := !(int size) -> void^ declare
free := !(void^ point) -> void declare

ve := class
{
	x,y,z := int
}

main := !(int argc, char^^ argv) -> int
{
	waw := ve
	waw.y = 88
	printf("y = %i\n",waw.y)
	return 0
}
