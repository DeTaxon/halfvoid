#import "lib.cp"
//#import "main.cp"


Test := !( a, b) -> void
{
	printf("b\n")
}

main := !(int argc, char^^ argv) -> int
{
	Test(2,3)
	Test(2,3.0)
	Test(2,7)
	return 0
}

