printf := !(char^ a,...) -> int declare

main := !(int argc, char^^ argv) -> int
{
	c := Tuple.{s16,s16}
	c = !{3,4}
	printf("a?  %i %i\n",c.0,c.1)
	//c.2 = 3
	//c.3 = "qwe"
	return 0
}

