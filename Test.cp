printf := !(char^ str, ...) -> int declare
calloc := !(int size,int sizet) -> void^ declare
malloc := !(int size) -> void^ declare
free := !(void^ point) -> void declare

ve := class
{
	x,y,z := int
	print := !() -> void
	{
		printf("hello method %i %i %i\n",z)
	}
}

main := !(int argc, char^^ argv) -> int
{
	waw := new ve
	waw.print()
	return 0
}
