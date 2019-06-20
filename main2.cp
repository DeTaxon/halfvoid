printf := !(char^ a,...) -> int declare

main := !(int argc, char^^ argv) -> int
{
	c := !{1->{s16},2}
	c.0 = 7
	c = !{3,4}
	printf("a?  %i %i\n",c.0,c.1)
	//c.2 = 3
	//c.3 = "qwe"
	return 0
}

