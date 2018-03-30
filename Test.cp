printf := !(char^ str, ...) -> int declare
calloc := !(int size,int sizet) -> void^ declare
malloc := !(int size) -> void^ declare
free := !(void^ point) -> void declare

ve := class
{
	x,y,z := int
	print := !(int u) -> void
	{
		printf("hello method %i %i %i %i\n",x,y,z,u)
	}
	print2 := !() -> void
	{
		print(15)
	}
}

main := !(int argc, char^^ argv) -> int
{
	waw := new ve
	waw.y = 20
	waw.print2()
	return 0
}
