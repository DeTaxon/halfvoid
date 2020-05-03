

main := !() -> int
{
	tst := AVLMap.{int,int}()

	tst[0] = 0
	tst[13] = 0
	tst[7] = 0
	tst[6] = 0
	tst[5] = 0
	tst[27] = 0
	tst[28] = 0
	for i,j : tst ; $reverse
	{
		printf("wut %i\n",j)
	}
	return 0
}
