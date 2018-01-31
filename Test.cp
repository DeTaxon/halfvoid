printf := !(char^ str, ...) -> int declare

//#run y = xml.load("asd.xml").value("size")

wow := !(int x) -> int
{
	printf(" value %i\n",x)
	return 0
}

main := !(int argc, char^^ argv) -> void
{
	//x := #select * #from y #where y in z #end
	//sp := type char^
	//x,y,z := 0
	//y =  7.1..9.5
	//g += 1.0 - 7.1de ** 3 + 7
	//o := new int()
	//orer := true or false
	//printf("Hello, word!%f\n",4.9..5.9)
	wow(3)
	y := 13

	while y >= 0
	{
		printf("y = %i\n",y)
		y = y - 1
	}
	//return 0
}
