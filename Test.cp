#import "lib.cp"
//#import "main.cp"

"![]" := !(int x, int y) -> int[2]
{
	ToRet[0] = x
	ToRet[1] = y
}

main := !(int argc, char^^ argv) -> int
{
	z := ![5,6]&
	printf("wow %i %i\n",z[0],z[1])
	return 0
	//return main2(argc,argv)
}





