

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

DeferFor := !() -> void
{
	val := 0
	for i : 4
	{
		defer val *= 2
		if i == 2
			break
		val += 1
	}
	assert(val == 12)

	
	val = 0
	for i : 4
	{
		defer val *= 2
		if i == 2
			continue
		val += 1
	}
	assert(val == 26)
}

DeferInLambda := !() -> void //TODO
{
	fnc := () ==> {
		defer printf("bob\n")
		//printf("hello\n")
		//throw new Exception("")
		return void
	}
	try{
		fnc()
	}catch(IException^ e){

	}
}

deferInTryVal := 0
DeferInTryThrow := !(bool doThrow) -> void
{
	deferInTryVal = 1
	if(doThrow)
		throw new Exception("")
}

DeferInTry1 := !(bool doThrow) -> void
{
	on_exception deferInTryVal += 1
	defer deferInTryVal *=2
	DeferInTryThrow(doThrow)
}

DeferInTry0 := !(bool doThrow) -> void
{
	try{
		defer deferInTryVal *= 2
		DeferInTry1(doThrow)
	}catch(IException^ e)
	{
		deferInTryVal += 1
	}
}
DeferInTry := !() -> void
{
	DeferInTry0(true)
	assert(deferInTryVal == 7)
	DeferInTry0(false)
	assert(deferInTryVal == 4)
}

DeferTest := !() -> void
{
	//DeferTestIf()
	//DeferWhile()
	//DeferFor()
	//DeferInTry()
	DeferInLambda()
}

//DeferAndYield
//DeferInLambda
//DeferGC


main := !(int argc, char^^ argv) -> int
{
	DeferTest()
	//DeferTest()

	//if false
	//{
	//	defer printf("bab\n")
	//	printf("ye\n")
	//	return 0
	//}else{
	//	defer printf("bib\n")
	//	printf("ye\n")
	//}
	//printf("hello\n")
}
