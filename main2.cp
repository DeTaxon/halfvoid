

//_blockStepUp := !(bool isException) -> void
//{
//	
//}
//_fullUnwind := !() -> void
//{
//}

DeferTest1Func := !(int val,int^ result) -> void
{
	defer result^ += 7
	result^ = 3

	on_exception result^ = 99999

	defer result^ *= 2
	if val == 0
	{
		defer result^ += 8
		result^ *= 2
	}
}
DeferTest1 := !() -> void
{
	x := 13
	DeferTest1Func(0,x&)
	assert(x == 35)
}


DeferTest := !() -> void
{
	DeferTest1()
}

main := !(int argc, char^^ argv) -> int
{
	//DeferTest()
	defer printf("bob\n")

	if argc != 255
	{
		defer printf("bab\n")
		printf("ye\n")
		return 0
	}
	printf("hello\n")
}
