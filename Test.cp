#import "lib.cp"
//#import "main.cp"


TT := class
{
	a := int

	"^" := !() -> void
	{
		printf("its work %i\n",a)
	}
	"+=" := !(int c) -> void
	{
		a += c
	}
}


main := !(int argc, char^^ argv) -> int
{
	k := TT
	k.a = 87
	k^
	k += 15
	k^
	return 0
	//return main2(argc,argv)
}

