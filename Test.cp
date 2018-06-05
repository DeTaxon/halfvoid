#import "lib.cp"
//#import "main.cp"


Test := !(@T a) -> void
{
	x := T
	printf("b\n")
}

main := !(int argc, char^^ argv) -> int
{	
	Test(2)
	Test(2.5)
	return 0
}

