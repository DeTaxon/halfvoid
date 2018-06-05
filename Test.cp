#import "lib.cp"
//#import "main.cp"


T2 := class
{
	start := int
	end := int
	this := !(int s, int e) -> void
	{
		start = s
		end = e
	}
	"^" := !() -> ref int
	{
		return start
	}
	IsEnd := !() -> bool
	{
		return start >= end
	}
	Inc := !() -> void
	{
		start += 2
	}
}
T1 := class
{
	x := int
	this := !(int j) -> void 
	{
		x = j
	}
	For := !() -> T2
	{
		return T2(0,x)
	}
}


main := !(int argc, char^^ argv) -> int
{
	for T1(7) printf("wow %i\n", it)
	return 0
}

