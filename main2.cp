
simplTest := !(!(float)&->bool tstObj) -> void
{
	printf("test %i\n",tstObj(12))
}


main := !(int argc, char^^ argv) -> int
{
	bugTest1()
	return 0
	x := 10000
	vals := List.{Tuple.{double,int}}() ; $keep
	//simplTest(_1 > x)
	simplTest( y ==> {
		printf("wut %i\n",x)
		return x > y
	})

	return 0

	////AppendClassTest()
	////return 0
	//TaskTest()
	//return 0
	
	try
	{
		TestSpaceship()
		TestSpaceshipTuple()
		InsertBeforeTest()
		BestTest()
		TaskTest() // Must be last
		printf("all good\n")
	}catch(IException^ e)
	{
		printf("Msg: %s\n",e.Msg())
	}
	return 0
}
printBool := !(bool toPrt) -> void
{
	printf("bool %i\n",toPrt)
}
bugTest1 := !() -> void
{
	x := 3
	printBool(x > 3 and  3 > x)
}
TestSpaceshipTuple := !() -> void
{
	assert(!{1,1} <=> !{1,1} == 0)
	assert(!{2,1} <=> !{1,1} == 1)
	assert(!{1,1} <=> !{2,1} == -1)
	assert(!{1,2} <=> !{2,1} == -1)
	assert(!{2,1} <=> !{1,2} == 1)

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
	assert(ptr1 <=> ptr2 == -1)
	assert(ptr1 <=> ptr1 == 0)
	assert(ptr2 <=> ptr1 == 1)
	//return 0
	assert(false <=> false == 0 )
	assert(false <=> true == 1)
	assert(true <=> false == -1)
	assert(true <=> true == 0)
}



taskTestValue := task_local int
gr := task_local double


TaskTest := !() -> void
{
	tb := CreateTaskBox()
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
		
	})
	tb.Spawn(() ==> {
		taskTestValue = 100
		for 2
		{
			printf("waw %i\n",taskTestValue++)
			TSleep(1)
		}
		
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
