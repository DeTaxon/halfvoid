#import "lib.cp"
//#import "main.cp"


Test2 := !()^ -> void

Test := !(@T a) -> void
{
	printf("b\n")
}

main := !(int argc, char^^ argv) -> int
{	
	Test2()
	Test(2)
	Test(2.5)
	return 0
}

