#import "lib.cp"
//#import "main.cp"


Test := !(@T a,T b) -> void
{
	printf("b\n")
}

main := !(int argc, char^^ argv) -> int
{
	Test(2,6.0)
	return 0
}

