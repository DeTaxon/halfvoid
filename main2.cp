


A := class 
{
	x := int
	foo := virtual !() -> void
	{
		printf("hoh\n")
		foo()
	}
}


main := !(int argc, char^^ argv) -> int
{
	tt := new B
	tt.foo()
	return 0
}

