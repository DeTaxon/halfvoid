
//libjit := Library
main := !(int argc, char^^ argv) -> int
{
	gRepo.Init(".")
	gRepo.AddZipRoot(argv[0])
	if $debug
	{
		gRepo.AddZipRoot("halfvoid")
		gRepo.AddZipRoot("halfvoid.exe")
	}
	libjitinit()
	printf("mod test %i\n",JIT_TYPE_PTR)
	printf("mod test %p\n",jit_context_create)
	return 0
	
	//libjit.Open("libjit.so")
	//jit_context_create := libjit.Get("jit_context_create")->{!()^->void^}
	//jit_context_build_start := libjit.Get("jit_context_build_start")->{!(void^)^->void^}
	////jit_type_ := libjit.Get("jit_type_")
	//jit_type_void := libjit.Get("jit_type_void")->{void^^}^
	//jit_type_int := libjit.Get("jit_type_int")->{void^^}^
	//jit_type_void_ptr := libjit.Get("jit_type_void_ptr")->{void^^}^
	//jit_type_create_signature := libjit.Get("jit_type_create_signature")->{!(int,void^,void^,int,int)^->void^}
	//jit_function_create := libjit.Get("jit_function_create")->{!(void^)^->void^}
	//jit_value_get_param := libjit.Get("jit_value_get_param")->{!(void^,int)^->void^}

	//jit_abi_cdecl := 0

	//ctx := jit_context_create()
	//jit_context_build_start(ctx)

	//pars := void^[2]
	//pars[0] = jit_type_int
	//pars[1] = jit_type_int
	//funcptr := jit_type_create_signature(jit_abi_cdecl,jit_type_int,pars,2,1)

	//f := jit_function_create(funcptr)
	return 0
	//if "wow" == "wow"
	//{
	//	bob + bob
	//}
	//val := int[15]
	//WantIt(val)	
	//return 0
	try
	{
		TestBiggerSwitch()
		TestRetQ()
		StringEndTest()
		TestSpaceship()
		TestSpaceshipTuple()
		InsertBeforeTest()
		BestTest()
		TestBugs()
		TaskTest() // Must be last
		printf("all good\n")
	}catch(IException^ e)
	{
		printf("Msg: %s\n",e.Msg())
	}
	return 0
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
