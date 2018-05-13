#import "lib.cp"
//#import "main.cp"

//"![]" := !(int x, int y) -> int[2]
//{
//	ToRet[0] = x
//	ToRet[1] = y
//}

main := !(int argc, char^^ argv) -> int
{
	x := new int[5]
	x[0] = 1
	h := 0
	h = h - 1
	// TODO h -= 1
	// TODO x[-1]
	printf("test %i %i %i\n", x[0],x[h],h )
	return 0
	//return main2(argc,argv)
}

