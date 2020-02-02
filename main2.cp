
g2 := !(!()&->void thr) -> void^
{
	z := thr->{void^}
	return z
}

main := !() -> int
{
	t2 := g2(() ==> {
		printf("heh hoh\n")
	})
	t2->{!()&->void}()
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
