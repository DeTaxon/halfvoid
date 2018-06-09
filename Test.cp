#import "lib.cp"
//#import "main.cp"


Test := !(@T a,T b) -> void
{
	printf("b\n")
}

main := !(int argc, char^^ argv) -> int
{
	Test(2,3)
	return 0
}

