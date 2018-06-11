#import "lib.cp"
//#import "main.cp"


Test := class
{
	x,y,z := int
	ho := !() -> void
	{
		y = 5
	}
}


main := !(int argc, char^^ argv) -> int
{
	T := Test
	T.ho()
	return 0
}

