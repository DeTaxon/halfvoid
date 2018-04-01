printf := !(char^ str, ...) -> int declare
calloc := !(int size,int sizet) -> void^ declare
malloc := !(int size) -> void^ declare
free := !(void^ point) -> void declare

ve := class
{
	x,y,z := float
	this := !()
	{
	}
	print := !(int u) -> void
	{
		printf("hello method %f %f %f %i\n",x,y,z,u)
	}
	print2 := !() -> void
	{
		print(15)
	}
}

ve2 := class extend ve
{
	w := int
	this := !()
	{
		w = 0
	}
}
main := !(int argc, char^^ argv) -> int
{
	Test := new ve()
	Test2 := new ve2()
	Test2.print2()
	return 0
}
