#import "lib.cp"
//#import "main.cp"


T2 := class
{
	a := int

	"^" := !() -> int
	{
		return a
	}
	Ind := !() -> int
	{
		return a
	}
	Inc := !() -> void
	{
		a += 2
	}
	IsEnd := !() -> bool
	{
		return a >= 10
	}
}
T1 := class
{
	a := bool
	For := !() -> T2
	{	
		ToRet.a = 0
	}
}


main := !(int argc, char^^ argv) -> int
{
	T := T1
	for i : T printf("pleas %i\n",i)
	return 0
	//return main2(argc,argv)
}

