
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
	keepStack := bool
	taskLocalPtr := void^
}

TaskKeepStackData := !() -> void
{
	assert(CurrentTaskBox != null)
	CurrentTaskBox.TaskKeepStackData()
}


TaskBox := class
{
	firstRunTasks := List.{TaskData^} ; $keep
	itStacks := List.{void^} ; $keep
	keptStacks := List.{void^} 

	stackSize := int

	itMutex := Mutex


	tasksToExe := List.{TaskData^} ; $keep

	destroyTasks := List.{TaskData^} ; $keep

	working := bool


	this := !(int sSize) -> void
	{
		stackSize = sSize
		itMutex."this"()
		itWorkMutex."this"()
		itWorkConVar."this"()
		itWorkCount = 0

		osInit()
		working = true
	}
	
	TaskKeepStackData := !() -> void
	{
		CurrentTask?.keepStack = true
	}

	pausedIdIter := int
	pausedProcesses := RBMap.{int,TaskData^} ; $keep
	PauseTask := !(int^ resId) -> void
	{
		pausedIdIter += 1
		while pausedIdIter in pausedProcesses
			pausedIdIter += 1
		resId^ = pausedIdIter
		pausedProcesses[pausedIdIter] = CurrentTask
		switchToMain()
	}
	ResumeTask := !(int procToken) -> void
	{
		if procToken in pausedProcesses
		{
			tasksToExe << pausedProcesses[procToken]
			pausedProcesses.Remove(procToken)
		}
	}
	Spawn := !(!()&->void tskToRun) -> void
	{
		nwTask := new TaskData
		nwTask.tskToRun = tskToRun.Capture()
		firstRunTasks << nwTask
	}

	
	switchToMain := !() -> void
	{
		_TaskPtrReset()
		osSwitchToMain()
	}
	doTask := !(TaskData^ toRun) -> void
	{
		CurrentTask = toRun
		_TaskPtrSet(toRun.taskLocalPtr)
		osDoTask(toRun)
	}
	onDestroyTask := !(TaskData^ toDestr) -> void
	{
		itMutex.Lock()
		destroyTasks << toDestr
		itMutex.Unlock()
		//TODO
		//delete toDestr
	}
	Quit := !() -> void
	{
		working = false
		notifyMain()
	}
	Run := !() -> void
	{
		
		CurrentTaskBox = this&
		working = true
		while working
		{
			makeWait := false
			waitTime := double
			toDoTask := TaskData^()

			itMutex.Lock()

			monitorPushWork()

			if destroyTasks.Size() != 0
			{
				for it : destroyTasks
				{
					if it.keepStack
					{
						if $posix keptStacks.Push(it.stackPtr)
						if $win32 keptStacks.Push(it.fiber)
						continue
					}
					it.tskToRun.Destroy()
					if $posix
					{
						itStacks.Push(it.stackPtr)
					}
					if $win32
					{
						DeleteFiber(it.fiber)
					}
				}
				destroyTasks.Clear()
			}

			toDoTask = checkCreateTask()

			if toDoTask == null
			{
				toDoTask = checkTimers(makeWait,waitTime)
				if toDoTask == null
				{
					toDoTask = checkExeWorks()
					if toDoTask == null
					{
						toDoTask = checkYields();
					}else{
						itWorkCount -= 1
					}
				}
			}

			if toDoTask != null
			{
				itMutex.Unlock()
				doTask(toDoTask)
				continue
			}

			if not makeWait and (itWorkCount != 0 or monitorWds.Size() != 0)
			{
				makeWait = true
				waitTime = 1
			}

			if makeWait
			{
				//itConVar.WaitFor(itMutex&,waitTime)
				itMutex.Unlock()
				taskWaitSleep(waitTime)
				continue
			}
			itMutex.Unlock()
			break
		}
		CurrentTask = null
		CurrentTaskBox = null

		itWorkMutex.Lock()
		working = false
		itWorkConVar.NotifyAll()
		itWorkMutex.Unlock()
		poolThread[^].Join()
	}
	checkCreateTask := !() -> TaskData^
	{
		if firstRunTasks.Size() == 0
			return null

		startTask := firstRunTasks.Pop()

		osCreateTask(startTask)
		startTask.taskLocalPtr = calloc(_getTaskStructSize(),1)
		_taskInitMem(startTask.taskLocalPtr)
		return startTask
	}
}
CreateTaskBox := !(int stackSize) -> TaskBox^
{
	return new TaskBox(stackSize)
}

ucontextStartTask := !(void^ fiberData) -> void
{
	CurrentTask.tskToRun()
	CurrentTaskBox.onDestroyTask(CurrentTask)
	CurrentTaskBox.switchToMain()
}