
tst := !(!(int)&->bool ifif) -> void
{
	for 1..10
	{
		if ifif(it)
			printf("tst %i\n",it)
	}
}

main := !() -> int
{
	tst(x ==> x % 2 == 0)
	return 0
}
