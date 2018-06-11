#import "lib.cp"
//#import "main.cp"


Test := class
{
	x,y,z := float
	j := float[3] at x
	ho := !() -> void
	{
		y = 5.0
		printf("wut %f\n",j[1])
	}
}


main := !(int argc, char^^ argv) -> int
{
	T := Test
	T.ho()
	return 0
}

