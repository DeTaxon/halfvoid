printf := !(char^ a,...) -> void declare

main := !(int argc, char^^ argv) -> int
{
	c := !{1,6}
	k := c
	printf("heh %i %i\n",k.0,k.1)
	return 0
}

