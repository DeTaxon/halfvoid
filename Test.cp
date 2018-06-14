#import "lib.cp"
//#import "main.cp"

W2 := class
{
	start,end := int
	this := !(int c)
	{
		start = 0
		end = c
	}
	"^" := !() -> int
	{
		return start
	}
	Inc := !() -> void
	{
		start += 1
	}
	IsEnd := !() -> bool
	{
		return start >= end
	}
}
W := class 
{
	x := int
	this := !(int l) -> void
	{
		x = l
	}
	For := !() -> W2
	{
		ToRet.start = x
		ToRet.end = 10
	}
}

main := !(int argc, char^^ argv) -> int
{
	for i : W(0), j : W(6)
		printf("wut %i %i\n",i,j)
	return 0
}

