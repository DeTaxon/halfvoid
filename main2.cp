"in" := !(T this,@T[@S] ar) -> bool
{
	if ar[^] == this return true
	return false
}

main := !() -> int
{
	if "hello" in !["mario","Luigi","hello"]
	{
		printf("yiep\n")
	}
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
