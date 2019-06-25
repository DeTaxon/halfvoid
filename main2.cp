//printf := !(char^ a,...) -> void declare

main := !(int argc, char^^ argv) -> int
{
	k := AVLMap.{int,int}()
	k[1] = 0
	k[7] = 0
	k[1] = 0
	k[9] = 0
	k[-10] = 0
	for i,j : k ; $reverse
		printf("%i ",j)
	printf("\n")
	return 0
	c := !{1,6}
	k := c
	printf("heh %i %i\n",k.0,k.1)
	return 0
}

