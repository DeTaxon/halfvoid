#import "lib.cp"
//#import "main.cp"

main := !(int argc, char^^ argv) -> int
{
	w := int[5]
	printf("wow %i\n", w->len)
	return 0
	//return main2(argc,argv)
}

