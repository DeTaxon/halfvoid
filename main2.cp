
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
	tst2 := virtual  !() -> void
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
subTest := class extend tstKl
{
	k := int
}

main := !() -> int
{
	//tst3()
	//tst3()
	r := tstKl
	r2 := new subTest
	r.tst2()
	r2.tst2()
	r.tst2()
	r2.tst2()
	r.tst2()
	r2.tst2()
	r.tst2()
	r2.tst2()
	//tst(_1 % 2 == 0)
	return 0
}
