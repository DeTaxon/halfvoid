
gtr := !(char^ tst, ...) -> void
{
}


g2 := !(void^ to) -> void
{
}

main := !() -> int
{
	g2(gtr)
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
