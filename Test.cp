#import "lib.cp"
//#import "main.cp"


Test := class
{
	x,y,z := float
	j := float[3] at x
	this := !() -> void
	{
	}
	ho := virtual !() -> void
	{
		printf("wut %f\n",y)
	}
}


main := !(int argc, char^^ argv) -> int
{
	T := Test
	T.y = 5.7
	T.ho()
	return 0
}

