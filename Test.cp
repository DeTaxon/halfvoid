printf := !(char^ str, ...) -> int declare
calloc := !(int size,int sizet) -> void^ declare
malloc := !(int size) -> void^ declare
free := !(void^ point) -> void declare

ve := class
{
	x,y,z := int
	print := !() -> void
	{
		printf("hello method %i %i %i\n",this.x,this.y,this.z)
	}
	print2 := !() -> void
	{
		this.print()
	}
}

main := !(int argc, char^^ argv) -> int
{
	waw := ve
	if waw.x == 0 and waw.y == 0
		waw.print2()
	return 0
}
