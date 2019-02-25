


A := class 
{
	x := int
	foo := virtual !() -> void
	{
		printf("hoh\n")
		foo()
	}
}

B := class extend A
{
	foo := virtual !() -> void
	{
		printf("hah\n")
		this."A.foo"()
	}
}

main := !(int argc, char^^ argv) -> int
{
	tt := new B
	tt.foo()
	return 0
}

