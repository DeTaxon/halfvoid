

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

DeferWhileFunc := !(bool isTrue , int^ result) -> void
{
	result^ = 3
	defer result^ *= 2
	while isTrue
	{
		defer result^ *= 100
		result^ = 7
		break
	}
}
DeferWhile := !() -> void
{
	val := 0
	DeferWhileFunc(true,val&)
	assert(val == 1400)
	DeferWhileFunc(false,val&)
	assert(val == 6)
}


DeferTest := !() -> void
{
	DeferTestIf()
	DeferWhile()
}

//DeferFor
//DeferAndYield
//DeferInLambda
//DeferTryCatch
//DeferGC


main := !(int argc, char^^ argv) -> int
{
	DeferTest()
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
