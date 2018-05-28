#import "lib.cp"
//#import "main.cp"

test := !(@T^ x) -> int
{
	printf("wow\n")
	return 0
}

main := !(int argc, char^^ argv) -> int
{
	k := 5
	test(k)
	return 0
	//return main2(argc,argv)
}

