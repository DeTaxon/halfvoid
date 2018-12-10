A := class
{
	c := virtual !() -> void
	{
		printf("A\n")
	}
	r := virtual 4
	Name := !() -> void
	{
		printf("wowwow\n")
	}
}
B := class extend A
{
	c := virtual !() -> void
	{
		printf("B\n")
	}
	r := virtual 5
	Name := !() -> void
	{
		printf("wawwaw\n")
	}
}

CreateA := !() -> A^
{
	return new A
}
CreateB := !() -> A^
{
	return new B
}
main := !(int argc, char^^ argv) -> int
{
	a := CreateA()
	b := CreateB()
	a.c()
	b.c()
	printf("wut %i\n",a.r)
	printf("wut %i\n",b.r)
	a.Name()
	b.Name()
	return 0
}

