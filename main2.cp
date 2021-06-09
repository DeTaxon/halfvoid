
TstCl := class
{
	c := virtual !() -> void
	{

	}
}
Cl2 := class extend TstCl
{
}
Cl3 := class extend Cl2
{
}

main := !(int argc, char^^ argv) -> int
{
	r := new Cl2
	if r is in TstCl
	{
		printf("yep\n")
	}else{
		printf("nope\n")
	}
}
