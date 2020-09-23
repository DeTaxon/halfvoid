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


tst1 := int
tst2 := int

taskTestValue := task_local int
TaskTest := !() -> void
{
	tb := CreateTaskBox()
	tb.Spawn(() ==> {
		TaskPtr = tst1&
		taskTestValue = 0
		for 5
		{
			TaskPtr = tst1&
			printf("wow %i\n",taskTestValue)
			taskTestValue = taskTestValue + 1
			TSleep(1)
		}
		
	})
	tb.Spawn(() ==> {
		TaskPtr = tst2&
		taskTestValue = 100
		for 5
		{
			TaskPtr = tst2&
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
		tst.InsertBeforeIf(i,_1 < i)
	}
	minVal := -14
	for i : tst
	{
		if i < minVal
			throw new Exception("Not sorted correctly")
	}

}
