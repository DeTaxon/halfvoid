
//tst := !(!(int)&->bool ifif) -> void
//{
//	for 1..10
//	{
//		if ifif(it)
//			printf("tst %i\n",it)
//	}
//}

tst2 := !() -> void
{
	while true
	{
		printf("ic\n")
		yield void
		printf("jo\n")
		yield void
	}
}
main := !() -> int
{
	tst2()
	tst2()
	//tst(_1 % 2 == 0)
	return 0
}
