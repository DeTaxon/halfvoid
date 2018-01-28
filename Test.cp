printf := !(char^ str, ...) -> int declare

//#run y = xml.load("asd.xml").value("size")

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

	x := int
	x = 0

	while x <= 13
	{
		printf("x = %i\n",x)
		x = x + 1
	}

	if 5 >= 5 {
		printf("wow\n")
	}else
	{
		printf("waw\n")
	}
	printf("Hello, world! %i\n",5 >= 5 )
	//return 0
}
