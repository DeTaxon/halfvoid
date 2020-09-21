boj := !(!(int&)& -> void tstQ) -> void
{
	k := 0
	tstQ(k)
	printf("wow %i\n",k)
}

main := !(int argc, char^^ argv) -> int
{
	boj( x ==> x = 13)
	y := 3
	return 0
	//tst := List.{int}()
	//for i : ![3,17,9,34,7,-13]
	//{
	//	j := i
	//	tst.InsertBeforeIf(i,x ==> x < j)
	//}
	//printf("test %i\n",tst[^])
	//return 0
	TaskTest()
	return 0
	
	try
	{
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
	firstRunTasks := List.{TaskData^} ; $keep
	itStacks := List.{void^} ; $temp

	stackSize := int
	startContext := u8[1024]
	Spawn := !(!()&->void tskToRun) -> void
	{
		nwTask := new TaskData
		nwTask.tskToRun = tskToRun
		firstRunTasks << nwTask
	}
	ASleep := !(double sleepTime) -> void
	{
	}
	Run := !() -> void
	{
		stackSize = 8*1024
		
		CurrentTaskBox = this&
		getcontext(startContext&)
		while true
		{
			if firstRunTasks.Size() != 0
			{
				startTask := firstRunTasks.Pop()
				CurrentTask = startTask
				getcontext(startTask.uContext&)
				startTask.uContext[24]&->{u64^}^ = 0
				startTask.uContext[8]&->{void^^}^ = null
				startTask.uContext[16]&->{void^^}^ = malloc(stackSize)
				startTask.uContext[32]&->{u64^}^ = stackSize
				makecontext(startTask.uContext&,ucontextStartTask,0)
				swapcontext(startContext&,startTask.uContext&)
				continue
			}
			break
		}
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
			TSleep(1)
		}
		
	})
	tb.Spawn(() ==> {
		for 5
		{
			printf("waw\n")
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
