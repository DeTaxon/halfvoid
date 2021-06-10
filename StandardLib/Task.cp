
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

M_CREATED := 1
M_DELETED := 2
M_CHANGED := 4

MonitorCallback := type !(char^,int)&-> void

TaskBox := class
{
	sleepTasks := List.{Tuple.{double,TaskData^}} ; $keep
	firstRunTasks := List.{TaskData^} ; $keep
	itStacks := List.{void^} ; $keep
	keptStacks := List.{void^} 

	stackSize := int

	itMutex := Mutex

	itWorkToDoPre := List.{Tuple.{!()&->void,TaskData^}} ; $keep

	itWorkMutex := Mutex
	itWorkConVar := ConVar
	itWorkToDo := List.{Tuple.{!()&->void,TaskData^}} ; $keep
	itWorkCount := int
	poolThread := List.{Thread^}

	tasksToExe := List.{TaskData^} ; $keep

	destroyTasks := List.{TaskData^} ; $keep

	yieldedTasks := List.{TaskData^} ; $keep
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
	monitorsToAdd := List.{Tuple.{char^,MonitorCallback,int,bool}} ; $keep
	Monitor := !(char^ pathName,int modes,bool recursive,MonitorCallback callb) -> void
	{
		itMutex.Lock()
		monitorsToAdd.Emplace(pathName,callb,modes,recursive)
		itMutex.Unlock()
		notifyMain()
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
	Yield := !() -> void
	{
		yieldedTasks.Push(CurrentTask)
		switchToMain()
	}

	ASleep := !(double sleepTime) -> void
	{
		assert(sleepTime > 0)
		nextTime := TGetSteadyTime() + sleepTime
		itMutex.Lock()
		newObj := ref sleepTasks.CreateBeforeIf(_1.0 < nextTime)
		newObj.0 = nextTime
		newObj.1 = CurrentTask
		itMutex.Unlock()
		switchToMain()
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

			if monitorsToAdd.Size() != 0
			{
				while monitorsToAdd.Size() != 0
				{
					frst := ref monitorsToAdd.Front()
					addMonitor(frst.0,frst.1,frst.2,frst.3)
					monitorsToAdd.Pop()
				}				
			}

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

			if not makeWait and itWorkCount != 0
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
	checkExeWorks := !() -> TaskData^
	{
		if tasksToExe.Size() == 0
			return null

		return tasksToExe.Pop()
	}
	checkTimers := !(bool& makeWait,double& waitTime) -> TaskData^
	{
		if sleepTasks.Size() == 0
			return null
		nowTime := TGetSteadyTime()
		first := ref sleepTasks.Front()
		if first.0 <= nowTime
		{
			itTask := first.1
			sleepTasks.Pop()
			return itTask
		}else{
			makeWait = true
			waitTime = first.0 - nowTime
		}
		return null
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
	checkYields := !() -> TaskData^
	{
		if yieldedTasks.Size() == 0
			return null
		return yieldedTasks.Pop()
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