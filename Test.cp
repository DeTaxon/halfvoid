#import "lib.cp"
//#import "main.cp"

test := !(int y,@T x) -> int
{
	printf("wow\n")
	return 0
}

main := !(int argc, char^^ argv) -> int
{
	printf("ww\n")
	test(5,5)
	return 0
	//return main2(argc,argv)
}

