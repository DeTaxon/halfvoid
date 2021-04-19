
GCTest := class extend TGCObject
{
	nxt := TGCObject^
	//TODO: relative ptr, List.{GCTest}, return new GCTest, capture returned objects
	//TODO: MemoryPools, TemporaryPool
	Destroy := virtual !() -> void
	{
		printf("yay\n")
	}
	GCLastRef := virtual !() -> void
	{
		printf("wut\n")
	}
}
miniBad2 := !() -> int
{
	tst := new GCTest
	tst.nxt = new GCTest
}

tst2 := class 
{
	a := int
	b := float
	z := void^
	vf := poison virtual !() -> void
	{
		for name : this->Fields
		{
			printf("heh %s\n",name)
		}
	}
}




main := !(int argc, char^^ argv) -> int
{
	c := new tst2
	//miniBad2()
	c.vf()
	return 0
	
	try
	{
		TestQuestion()
		TestBiggerSwitch()
		TestRetQ()
		StringEndTest()
		TestSpaceship()
		TestSpaceshipTuple()
		InsertBeforeTest()
		BestTest()
		TestBugs()
		TestSetType()
		TaskTest() // Must be last
		printf("all good\n")
	}catch(IException^ e)
	{
		printf("Msg: %s\n",e.Msg())
	}
	return 0
}

classSetTypeA := class
{
	x := int
	Pt := virtual !() -> void
	{
		x = 13
	}
}
classSetTypeB := class extend classSetTypeA
{
	Pt := virtual !() -> void
	{
		x = 27
	}
}

TestQuestion := !() -> void
{
	a1 := AVLMap.{int,int}()
	a1[3] = 100
	a2 := AVLMap.{int,int}()
	a2[4] = 120
	sum := 0
	for i : 10
	{
		sum += a1.TryFind(i)?^ ?? a2.TryFind(i)?^ ?? 2
	}
	assert(sum == 236)

	tst2 := OnOddTrue
	tst2.x = 17
	assert(tst2[14] == 68)
	assert(tst2[14]? == 0)
	assert(tst2.x == 67)
}
OnOddTrue := class
{
	x := int
	"[]?" := !(int ind) -> ref int
	{
		x = 67
		if ind != 13
			return null
		return x
	}
	"[]" := !(int ind) -> ref int
	{	
		x = 68
		return x
	}
}


TestHashMap := !() -> void
{
	tst := HashAVLMap.{char^,int}()
	tst["hello"] = 1
	tst["world"] = 1
	tst["azb"] = 1
	tst["zb"] = 1
	tst["world"] = 2
	tst["1world"] = 2
	tst["2world"] = 2
	tst["2world"] = 3
	assert(tst.Size() == 6)
}
TestHashMap := !() -> void
{
	tst := HashAVLMap.{char^,int}()
	tst["hello"] = 1
	tst["world"] = 1
	tst["azb"] = 1
	tst["zb"] = 1
	tst["world"] = 2
	tst["1world"] = 2
	tst["2world"] = 2
	tst["2world"] = 3
	assert(tst.Size() == 6)
}
TestSetType := !() -> void
{
	j := new classSetTypeA
	j.Pt()
	assert(j.x == 13)
	j->SetType(classSetTypeB)
	j.Pt()
	assert(j.x == 27)
}

FuncPointerWithNames := !(char^ name)^ -> void

WantIt := !(@Typ x) -> void
{
	y := Typ->Base
	y = 13
	printf("heh %i %i\n",Typ->Len,y)
}
globInt := int
retrVal := !(bool retNull) -> int^
{
	if retNull
		return null
	return globInt&
}
TestRetQ := !() -> int^
{
	h := 0
	defer assert(h == 26)
	return retrVal(true)?
	h = 26
	return retrVal(false)?
	assert(false)
}

TestBiggerSwitch := !() -> void
{
	x := 13
	y := 0
	switch x
	{
		case 13 continue
		case 10
			y = 1
	}
	assert(y == 1)
}
StringEndTest := !() -> void
{
	start := "sample text"
	end := StringEnd(start)

	assert(end <=> "text" == 0)
	assert(end <=> "ext" == 0)
	assert(end <=> "test" > 0)
}

TestBugs := !() -> void
{
	bugTest1()
}
bugTest1 := !() -> void
{
	x := 4
	//and did not
	assert(x > 3 and 10 > x)
}
TestSpaceshipTuple := !() -> void
{
	assert(!{1,1} <=> !{1,1} == 0)
	assert(!{2,1} <=> !{1,1} == 1)
	assert(!{1,1} <=> !{2,1} < 0)
	assert(!{1,2} <=> !{2,1} < 0)
	assert(!{2,1} <=> !{1,2} > 0)

	assert(!{1,1} == !{1,1})
	assert(!{2,1} > !{1,1})
	assert(!{1,1} < !{2,1})
	assert(!{1,2} < !{2,1})
	assert(!{2,1} > !{1,2})
}
TestSpaceship := !() -> void
{
	someArr := int[3]
	ptr1 := someArr[0]&
	ptr2 := someArr[1]&
	assert(ptr1 <=> ptr2 < 0)
	assert(ptr1 <=> ptr1 == 0)
	assert(ptr2 <=> ptr1 == 1)
	//return 0
	assert(false <=> false == 0 )
	assert(false <=> true > 0)
	assert(true <=> false < 0)
	assert(true <=> true == 0)
}



taskTestValue := task_local int
gr := task_local double


TaskTest := !() -> void
{
	tb := CreateTaskBox(1024*1024)
	tb.ExpectWorkers(1)
	//tb.Monitor(".",true,(x) ==>{
	//	printf("test %s\n",x)
	//})
	tb.Spawn(() ==> [tb]{
		taskTestValue = 0
		for 4
		{
			printf("wow %i\n",taskTestValue)
			taskTestValue = taskTestValue + 1
			AwaitWork(() ==> {
				TSleep(1)
			})
		}
		assert(taskTestValue == 4)
		
	})
	tb.Spawn(() ==> {
		taskTestValue = 100
		for 2
		{
			printf("waw %i\n",taskTestValue++)
			TSleep(1)
		}
		assert(taskTestValue = 100)
		
	})
	tb.Run()
}
ToAppendClass := class 
{
	getValue := !() -> int
	{
		return getValue2()*13
	}
}
AppendClass ToAppendClass
{
	getValue2 := !() -> int
	{
		return 5
	}
}

AppendClassTest := !() -> void
{
	tstObj := ToAppendClass
	retVal := tstObj.getValue()
	assert(retVal == 5*13)
}

BestTest := !() -> void
{
	on_exception printf("BestTest\n")

	v1 := 3.5->{#best(int,float)}
	v2 := v1 - 3.5
	if v2 < 0 v2 = -v2
	assert(v2 < 0.1)
}

InsertBeforeTest := !() -> void
{
	tst := List.{int}()
	for i : ![3,17,9,34,7,-13]
	{
		tst.CreateBeforeIf(_1 < i) = i
	}
	minVal := -14
	for i : tst
	{
		if i < minVal
			throw new Exception("Not sorted correctly")
	}

}
