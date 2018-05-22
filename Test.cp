#import "lib.cp"
//#import "main.cp"

main := !(int argc, char^^ argv) -> int
{
	w := 5..9
	printf("wow %i %i\n", w->begin,w->end)
	return 0
	//return main2(argc,argv)
}

