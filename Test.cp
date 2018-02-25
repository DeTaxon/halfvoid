printf := !(char^ str, ...) -> int declare
calloc := !(int size,int sizet) -> void^ declare
malloc := !(int size) -> void^ declare
free := !(void^ point) -> void declare

ve := class
{
	x,y,z := int
	print := !()
	{
		printf("hello method\n")
	}
}

main := !(int argc, char^^ argv) -> int
{
	waw := ve^
	waw = new ve
	waw.print()
	free(waw)
	return 0
}
