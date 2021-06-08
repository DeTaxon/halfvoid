
TstCl := class
{

}
AppendClass TstCl
{
	fnc := virtual !() -> void
	{
		printf("hey\n")
	}	
}
Cl2 := class extend TstCl
{
	fnc2 := virtual !() -> void
	{
		c = 7
	}
}
AppendClass Cl2
{
	c := int
	fnc := virtual !() -> void
	{
		printf("hop\n")
	}	
}


main := !(int argc, char^^ argv) -> int
{
	r := new Cl2
	r.fnc2()
	printf("%i\n",r.c)
}
