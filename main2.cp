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

getcontext := !(void^ a) -> int declare
setcontext := !(void^ a) -> int declare
makecontext := !(void^ a,!()^->void  b,int argc,...) -> int declare
swapcontext := !(void^ begin, void^ end) -> int declare

TaskField := thread_local bool
CurrentTask := thread_local TaskData^
CurrentTaskBox := thread_local TaskBox^
TaskData := class
{
	tskToRun := !()& -> void
	uContext := u8[1024]
}
ucontextStartTask := !() -> void
{
	CurrentTask.tskToRun()
	setcontext(CurrentTaskBox.startContext&)
}
TaskBox := class
{
	sleepTasks := List.{Tuple.{double,TaskData^}} ; $keep
	firstRunTasks := List.{TaskData^} ; $keep
	itStacks := List.{void^} ; $temp

	stackSize := int
	startContext := u8[1024]

	itMutex := Mutex
	itConVar := ConVar

	this := !() -> void
	{
		itMutex."this"()
		itConVar."this"()
	}
	Spawn := !(!()&->void tskToRun) -> void
	{
		nwTask := new TaskData
		nwTask.tskToRun = tskToRun
		firstRunTasks << nwTask
	}
	ASleep := !(double sleepTime) -> void
	{
		assert(sleepTime > 0)
		nextTime := TGetSteadyTime() + sleepTime
		newObj := ref sleepTasks.CreateBeforeIf(x ==> x.0 < nextTime)
		newObj.0 = nextTime
		newObj.1 = CurrentTask
		swapcontext(CurrentTask.uContext&,startContext)
	}
	doTask := !(TaskData^ toRun) -> void
	{
		CurrentTask = toRun
		swapcontext(startContext&,toRun.uContext&)
	}
	Run := !() -> void
	{
		stackSize = 8*1024
		
		CurrentTaskBox = this&
		while true
		{
			makeWait := false
			waitTime := double
			if firstRunTasks.Size() != 0
			{
				startTask := firstRunTasks.Pop()
				getcontext(startTask.uContext&)
				startTask.uContext[24]&->{u64^}^ = 0
				startTask.uContext[8]&->{void^^}^ = null
				startTask.uContext[16]&->{void^^}^ = malloc(stackSize)
				startTask.uContext[32]&->{u64^}^ = stackSize
				makecontext(startTask.uContext&,ucontextStartTask,0)
				doTask(startTask)
				continue
			}
			if sleepTasks.Size() != 0
			{
				nowTime := TGetSteadyTime()
				first := ref sleepTasks.Front()
				if first.0 < nowTime
				{
					itTask := first.1
					sleepTasks.Pop()
					doTask(itTask)
					continue
				}else{
					makeWait = true
					waitTime = first.0 - nowTime
				}
			}
			if makeWait
			{
				itMutex.Lock()
				itConVar.WaitFor(itMutex&,waitTime)
				itMutex.Unlock()
				continue
			}
			break
		}
		printf("quit\n")
	}
}
CreateTaskBox := !() -> TaskBox^
{
	return new TaskBox
}

TaskTest := !() -> void
{
	tb := CreateTaskBox()
	tb.Spawn(() ==> {
		for 5
		{
			printf("wow\n")
			CurrentTaskBox.ASleep(1)
		}
		
	})
	tb.Spawn(() ==> {
		for 5
		{
			printf("waw\n")
			CurrentTaskBox.ASleep(1)
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
