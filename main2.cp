

main2 := !() -> int
{
	tst := AVLMap.{int,int}()

	tst[1] = 0
	tst[0] = 0
	for i,j : tst ; $reverse
	{
		printf("wut %i\n",j)
	}
	return 0
}
main := !() -> int
{
	return main2()
}
