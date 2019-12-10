tst := class
{
	x := int
	y := double
}

Cll := !(tst hh) -> void
{
	printf("wow %i %f\n",hh.x)
}

main := !() -> int
{
	j := tst
	j.x = 7
	Cll(j)
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
