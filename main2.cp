
FuncCall := !(int x) -> void
{
	printf("funca a\n")
}

FuncCall := !(int x) -> void 
{
	printf("funca a\n")
}

CalcFunc := !() -> int
{
	printf("calculate\n")
	return 3
}

main := !() -> int
{

	FuncCall[^^](CalcFunc())
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
