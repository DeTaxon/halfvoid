cc := class
{
	st := char^
}

main := !() -> int
{
	c := cc
	c.st = "wow"

	switch c.st?
	{
		case "wow" 
			printf("w\n")
		case void
			printf("else\n")
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
