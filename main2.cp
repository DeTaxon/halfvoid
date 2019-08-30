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

B := class
{
	h := A ; #outer_class
}

main := !() -> int
{
	k := B ; $ttt
	k.h.pr()
	return 0
}

