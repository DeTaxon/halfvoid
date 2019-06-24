

main := !(int argc, char^^ argv) -> int
{
	srand(7)
	printf("start\n")
	c := AVLMap.{int,int}()
	for 15
	{
		w := rand() % 128

		c[w] = 0
	}

	for i,j : c
	{
		printf("%i ",j)
	}
	printf("\n")
	return 0
}

