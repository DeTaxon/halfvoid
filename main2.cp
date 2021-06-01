

//_blockStepUp := !(bool isException) -> void
//{
//	
//}
//_fullUnwind := !() -> void
//{
//}

DeferTestIfFunc := !(bool isTrue,int^ result) -> void
{
	defer result^ *= 2
	if isTrue
	{
		defer result^ *= 10
		result^ = 7
	}else{
		defer result^ *= 100
		result^ = 5
	}
	result^ += 3
}
DeferTestIf := !() -> void
{
	val := 0
	DeferTestIfFunc(true,val&)
	assert(val == 146)
	DeferTestIfFunc(false,val&)
	assert(val == 1006)
}

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
	DeferTestIf()
	//DeferTest()
	defer printf("bob\n")

	if false
	{
		defer printf("bab\n")
		printf("ye\n")
		return 0
	}else{
		defer printf("bib\n")
		printf("ye\n")
	}
	printf("hello\n")
}
