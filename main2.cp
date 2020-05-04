

main2 := !() -> int
{
	printf("heh %i\n",1 <=> 2)
	tst := AVLMap.{int,int}()

	tst[0] = 0
	tst[16] = 0
	tst[46] = 0
	tst[3] = 0
	tst[22] = 0
	tst[10] = 0
	tst[19] = 0
	tst[38] = 0
	tst[22] = 0

	for j,i : tst
	{
		printf("wut %i\n",i)
	}
	return 0
}
main := !() -> int
{
	return main2()
}
