printf := !(char^ a,...) -> int declare

main := !(int argc, char^^ argv) -> int
{
	c := Tuple.{s16,s16}(3,5)
	c = !{6,7}
	printf("a?  %i %i\n",c.0,c.1)
	//c.2 = 3
	//c.3 = "qwe"
	return 0
}

