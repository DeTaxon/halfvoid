

main := !() -> int
{
	x := 3

	lmbd := (int y) ==> {
		z := x + y
		printf("heh %i\n",z)
	}
	lmbd(4)
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
