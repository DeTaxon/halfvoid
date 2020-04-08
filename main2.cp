
//tst := !(!(int)&->bool ifif) -> void
//{
//	for 1..10
//	{
//		if ifif(it)
//			printf("tst %i\n",it)
//	}
//}

tstKl := class
{
	z := int
	tst2 := !() -> void
	{
		x := 7
		while true
		{
			printf("ic %i\n",x)
			x++
			yield void
			printf("jo %i\n",x)
			x++
			yield void
		}
	}
}

main := !() -> int
{
	r := tstKl
	r.tst2()
	r.tst2()
	r.tst2()
	r.tst2()
	//tst(_1 % 2 == 0)
	return 0
}
