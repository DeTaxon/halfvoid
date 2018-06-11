#import "lib.cp"
//#import "main.cp"


Test := class
{
	x,y,z := float
	j := float[3] at x
	ho := !() -> void
	{
		printf("wut %f\n",y)
	}
}


main := !(int argc, char^^ argv) -> int
{
	T := Test
	T.j[1] = 4.7
	T.ho()
	return 0
}

