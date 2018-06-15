#import "lib.cp"
//#import "main.cp"

W3 := class
{
	start,end := float
	this := !(float c)
	{
		start = 0.0f
		end = c
	}
	"^" := !() -> float
	{
		return start
	}
	Inc := !() -> void
	{
		start = start + 1.3f
	}
	IsEnd := !() -> bool
	{
		return start >= end
	}
}
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
N := class 
{
	x := float
	this := !(float l) -> void
	{
		x = l
	}
	"~For" := !() -> W3
	{
		ToRet.start = x
		ToRet.end = 10.0
	}
}
W := class 
{
	x := int
	this := !(int l) -> void
	{
		x = l
	}
	"~For" := !() -> W2
	{
		ToRet.start = x
		ToRet.end = 10
	}
}
"~For" := !(int x) -> W2
{
	ToRet.start = 0
	ToRet.end = x
}

main := !(int argc, char^^ argv) -> int
{
	for i : W(0), j : N(6.0f), k : 5
	{
		printf("wut %i %f %i\n",i,j,k)
	}
	return 0
}

