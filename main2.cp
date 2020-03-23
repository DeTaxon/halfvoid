
tst := !(int x) -> void
{
	z := 8
	printf("tst %i\n",x)
}

main := !() -> int
{
	tst(3)
	return 0
}


//tst := !(@T... args) -> T
//{
//	return 0.0
//}
//
//main := !() -> int
//{
//	x := tst(1.0f,3.0f,4)
//	return 0
//}
//
