

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

DeferInLambda := !() -> void
{
	vl := 0
	fnc := () ==> {
		//x := 27
		defer vl = 14
		//defer printf("hop %i\n",x)
		vl = 1
		//printf("hello\n")
		throw new Exception("")
		return void
	}
	try{
		fnc()
	}catch(IException^ e){
		vl *= 2
	}
	//printf("bl %i\n",vl)
	assert(vl == 28)
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

DeferInSwitch := !(int val) -> void
{
	defer printf("ye\n")

	{
		defer printf("sk\n")
	}
	switch(val)
	{
		case 1:
		{
			defer printf("hob\n")
			printf("heh\n")
		}
		case void
	}
}

DeferTest := !() -> void
{
	DeferTestIf()
	DeferWhile()
	DeferFor()
	DeferInTry()
	DeferInLambda()
	//DeferInSwitch(1)
	//DeferInSwitch(2)
}

innerSp := class 
{
	start,end := int
	this := !(int a,int b) .{} -> void
	{		start = a
			end = b
	}
	"^" := !() -> int
	{		return start	}
	Inc := !() .{} -> void
	{		start += 1	}
	IsEnd := !() .{} -> bool
	{		return start >= end	}
	Destroy := !() -> void
	{
		printf("yep\n")
	}
}

tstCl := class
{
	"~For" := !() .{} -> innerSp
	{
		return innerSp(0,2)
	}
}

//DeferAndYield
//DeferGC

BugTest := !() -> void
{
	lst := new List.{int}
	defer lst.Destroy()

	for i : 5
	{
		lst.Push(i)
		if i != 255
		{
			lst.Push(13)
		}
	}
	try{
		printf("bad\n")
	}catch(IException^ e){

	}
	if lst.Size() == 0
	{
		return void
	}
}


DeferAndYield := !() -> void
{
	defer printf("d\n")
	printf("a\n")
	yield void
	defer printf("c\n")
	printf("b\n")
	return  void
}

main := !(int argc, char^^ argv) -> int
{
	//DeferTest()
	//DeferAndYield()
	//DeferAndYield()

}
