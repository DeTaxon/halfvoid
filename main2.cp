
tst := !(@T... args) -> void
{
	printf("sz %f %f\n",args...)
}

main := !() -> int
{
	printf("heh\n")
	tst(1.0 ,2.0 )
	tst(4.0f,3.0 )
	tst(5.0 ,6.0f)
	tst(8.0f,7.0f)
	printf("hoh\n")
	return 0
}

