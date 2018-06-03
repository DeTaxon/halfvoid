#import "lib.cp"
//#import "main.cp"


T2 := class
{
	a := int
	p := int^

	"^" := !() -> ref int
	{
		return p[a] //a[p]
	}
	Ind := !() -> int
	{
		return a
	}
	Inc := !() -> void
	{
		a += 1
	}
	IsEnd := !() -> bool
	{
		return a >= 20
	}
}
T1 := class
{
	arr := int[20]
	For := !() -> T2
	{	
		ToRet.a = 0
		ToRet.p = arr
	}
}


main := !(int argc, char^^ argv) -> int
{
	T := T1
	for i : 20 T.arr[i] = i
	for i : T i += 20

	for i : T 
	{
		printf("pleas %i\n",i)
	}
	return 0
	//return main2(argc,argv)
}

