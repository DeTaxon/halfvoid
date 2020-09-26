
getcontext := !(void^ a) -> int declare
setcontext := !(void^ a) -> int declare
makecontext := !(void^ a,!()^->void  b,int argc,...) -> int declare
swapcontext := !(void^ begin, void^ end) -> int declare

ConvertThreadToFiber := !(void^ fbr) -> void^ declare
SwitchToFiber := !(void^ fbr) -> void declare 
CreateFiber := !(size_t stackSize,!(void^)^->void fiberFunc,void^ fiberValue) -> void^ declare
DeleteFiber := !(void^ fbr) -> void declare

CurrentTask := thread_local TaskData^
CurrentTaskBox := thread_local TaskBox^
TaskData := class
{
	tskToRun := !()& -> void
	if $posix
		uContext := u8[1024]
	if $posix
		stackPtr := void^
	if $win32
		fiber := void^
	taskLocalPtr := void^
}
ucontextStartTask := !(void^ fiberData) -> void
{
	CurrentTask.tskToRun()
	CurrentTaskBox.onDestroyTask(CurrentTask)
	CurrentTaskBox.switchToMain()
}

AwaitWork := !(!()&->void lambd) -> void
{
	if CurrentTaskBox != null
	{
		CurrentTaskBox.AwaitWork(lambd)
	}else{
		lambd()
	}
}

TaskBox := class
{
	sleepTasks := List.{Tuple.{double,TaskData^}} ; $keep
	firstRunTasks := List.{TaskData^} ; $keep
	itStacks := List.{void^} ; $keep

	stackSize := int

	if $posix
		startContext := u8[1024]
	if $win32
		startContext := void^

	itMutex := Mutex
	itConVar := ConVar

	itWorkToDoPre := List.{Tuple.{!()&->void,TaskData^}} ; $keep

	itWorkMutex := Mutex
	itWorkConVar := ConVar
	itWorkToDo := List.{Tuple.{!()&->void,TaskData^}} ; $keep
	itWorkDone := List.{TaskData^} ; $keep
	poolThread := List.{Thread^}

	working := bool

	this := !() -> void
	{
		itMutex."this"()
		itConVar."this"()
		itWorkMutex."this"()
		itWorkConVar."this"()

		if $win32
			startContext = ConvertThreadToFiber(null)
		working = true
	}
	Spawn := !(!()&->void tskToRun) -> void
	{
		nwTask := new TaskData
		nwTask.tskToRun = tskToRun.Capture()
		firstRunTasks << nwTask
	}
	ASleep := !(double sleepTime) -> void
	{
		assert(sleepTime > 0)
		nextTime := TGetSteadyTime() + sleepTime
		newObj := ref sleepTasks.CreateBeforeIf(x ==> x.0 < nextTime)
		newObj.0 = nextTime
		newObj.1 = CurrentTask
		switchToMain()
	}
	ExpectWorkers := !(int expects) -> void
	{
		diffWorks := expects - poolThread.Size()
		if diffWorks > 0
		{
			for i : diffWorks
			{
				someThis := this&
				newThread := new Thread(() ==> [someThis]{
					while someThis.working
					{
						someThis.itWorkMutex.Lock()

						if someThis.itWorkToDo.Size() != 0
						{
							frstWork := someThis.itWorkToDo.Front().0
							frstTask := someThis.itWorkToDo.Front().1
							someThis.itWorkToDo.Pop()
							someThis.itWorkMutex.Unlock()
							frstWork()
							someThis.itMutex.Lock()
							someThis.itWorkDone << frstTask
							someThis.itConVar.Notify()
							someThis.itMutex.Unlock()

						}else{
							someThis.itWorkConVar.Wait(someThis.itWorkMutex&)
							someThis.itWorkMutex.Unlock()
						}
					}
				})
				poolThread << newThread
			}
		}
	}
	AwaitWork := !(!()&->void lambd) -> void
	{
		itMutex.Lock()
		itWorkToDoPre.Emplace(lambd,CurrentTask)
		itMutex.Unlock()
		switchToMain()
	}
	switchToMain := !() -> void
	{
		_TaskPtr = null
		if $posix
			swapcontext(CurrentTask.uContext&,startContext)
		if $win32
			SwitchToFiber(startContext)
	}
	doTask := !(TaskData^ toRun) -> void
	{
		CurrentTask = toRun
		_TaskPtr = toRun.taskLocalPtr
		if $posix
			swapcontext(startContext&,toRun.uContext&)
		if $win32
			SwitchToFiber(toRun.fiber)

	}
	onDestroyTask := !(TaskData^ toDestr) -> void
	{
		toDestr.tskToRun.Destroy()
		if $posix
		{
			itStacks.Push(toDestr.stackPtr)
		}
		if $win32
		{
			DeleteFiber(toDestr.fiber)
		}
		//TODO
		//delete toDestr
	}
	Quit := !() -> void
	{
		working = false
		itConVar.Notify()
	}
	Run := !() -> void
	{
		stackSize = 8*1024
		
		CurrentTaskBox = this&
		working = true
		while working
		{
			makeWait := false
			waitTime := double

			itMutex.Lock()
			if itWorkToDoPre.Size() != 0
			{
				itWorkMutex.Lock()
				for it : itWorkToDoPre
				{
					itWorkToDo.Emplace(it.0,it.1)
				}
				itWorkToDoPre.Clear()
				itWorkConVar.NotifyAll()
				itWorkMutex.Unlock()
			}
			itMutex.Unlock()

			itWorkMutex.Lock()
			if itWorkDone.Size() != 0
			{
				ress := itWorkDone.ToArray()
				itWorkDone.Clear()
				doTask(ress[^])
			}
			itWorkMutex.Unlock()

			if firstRunTasks.Size() != 0
			{
				startTask := firstRunTasks.Pop()

				if $posix
				{
					getcontext(startTask.uContext&)
					startTask.stackPtr = malloc(stackSize)
					startTask.uContext[24]&->{u64^}^ = 0
					startTask.uContext[8]&->{void^^}^ = null
					startTask.uContext[16]&->{void^^}^ = startTask.stackPtr
					startTask.uContext[32]&->{u64^}^ = stackSize
					makecontext(startTask.uContext&,ucontextStartTask,0)
				}
				if $win32
				{
				      startTask.fiber = CreateFiber(stackSize->{s64},ucontextStartTask,null)
				}
				startTask.taskLocalPtr = calloc(_getTaskStructSize(),1)
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
		CurrentTask = null
		CurrentTaskBox = null
	}
}
CreateTaskBox := !() -> TaskBox^
{
	return new TaskBox()
}
