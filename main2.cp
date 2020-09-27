main := !(int argc, char^^ argv) -> int
{	
	TaskTest()
	return 0
	
	try
	{
		InsertBeforeTest()
		BestTest()
	}catch(IException^ e)
	{
		printf("Msg: %s\n",e.Msg())
	}
	return 0
}

taskTestValue := task_local int
gr := task_local double

TaskTest := !() -> void
{
	tb := CreateTaskBox()
	tb.ExpectWorkers(1)
	tb.Spawn(() ==> [tb]{
		taskTestValue = 0
		for 5
		{
			printf("test 1 %p %p\n",gTemporaryPool&,_TaskPtr)
			printf("wow %i\n",taskTestValue)
			taskTestValue = taskTestValue + 1
			AwaitWork(() ==> {
				printf("test await  %p %p\n",gTemporaryPool&,_TaskPtr)
				TSleep(1)
			})
		}
		
	})
	tb.Spawn(() ==> {
		taskTestValue = 100
		for 5
		{
			printf("test 2 %p %p\n",gTemporaryPool&,_TaskPtr)
			printf("waw %i\n",taskTestValue++)
			TSleep(1)
		}
		
	})
	tb.Run()
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
		//tst.InsertBeforeIf(i,_1 < i)
	}
	minVal := -14
	for i : tst
	{
		if i < minVal
			throw new Exception("Not sorted correctly")
	}

}
