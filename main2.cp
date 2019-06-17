printf := !(char^ a,...) -> int declare

main := !(int argc, char^^ argv) -> int
{
	c := !{1,2,"abc"}
	printf("a?  %i %i %s\n",c.0,c.1,c.2)
	//c.2 = 3
	//c.3 = "qwe"
	return 0
}

