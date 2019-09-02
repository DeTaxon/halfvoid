//printf := !(char^ a,...) -> int declare

A := class
{
	c := int
	pr := !() .{} -> void
	{
		if $ttt
		{
			printf("good\n")
		}else{
			printf("bad\n")
		}
	}
}

B := class extend A
{
}

main := !() -> int
{
	k := B ; $ttt
	k.pr()
	return 0
}

