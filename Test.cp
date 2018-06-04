#import "lib.cp"
//#import "main.cp"


T1 := class
{
	x := int
	this := !() -> void 
	{
		x = 5
	}
}


main := !(int argc, char^^ argv) -> int
{
	T := T1()
	printf("pleas %i\n",T.x)
	return 0
}

