
getcontext := !(void^ a) -> int declare
setcontext := !(void^ a) -> int declare
makecontext := !(void^ a,!()^->void  b,int argc,...) -> int declare
swapcontext := !(void^ begin, void^ end) -> int declare

inotify_init := !() -> int declare
inotify_add_watch := !(int fd,char^ pat,int mask) -> int declare
read := !(int fd,void^ dat,size_t sizeToRead) -> s64 declare

inotify_event := class
{
	wd := int
	mask := u32
	cookie := u32
	len := u32
	name := u8[1]
}

eventfd := !(u64 initVal,int flags) -> int declare

pollfd := class 
{
	fd := int
	events := u16
	revents := u16
}
poll := !(void^ pols, int count,int timeout) -> int declare

ConvertThreadToFiber := !(void^ fbr) -> void^ declare
SwitchToFiber := !(void^ fbr) -> void declare 
CreateFiber := !(size_t stackSize,!(void^)^->void fiberFunc,void^ fiberValue) -> void^ declare
DeleteFiber := !(void^ fbr) -> void declare

CreateEventA := !(void^ security,int manualReset,int bInitState,char^ pName) -> void^ declare
SetEvent := !(void^ evnt) -> int declare
ResetEvent := !(void^ evnt) -> int declare
WaitForMultipleObjects := !(u32 nCount,void^ handlers,int waitAll,u32 millicenodsTimeout) -> int declare
WAIT_OBJECT_0 := 0
WAIT_ABANDONED_0 := 0x80
WAIT_TIMEOUT := 0x102
//wait failed 0xfffffff

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

	itMutex := Mutex

	itWorkToDoPre := List.{Tuple.{!()&->void,TaskData^}} ; $keep

	itWorkMutex := Mutex
	itWorkConVar := ConVar
	itWorkToDo := List.{Tuple.{!()&->void,TaskData^}} ; $keep
	itWorkDone := List.{TaskData^} ; $keep
	poolThread := List.{Thread^}

	destroyTasks := List.{TaskData^} ; $keep

	working := bool

	pollData := RawArray.{Tuple.{int}}

	this := !(int sSize) -> void
	{
		stackSize = sSize
		itMutex."this"()
		itWorkMutex."this"()
		itWorkConVar."this"()

		pollData."this"()
		pollData.Reserve(10)

		osInit()
		working = true
	}
	
	
	initedMonitor := bool
	monitorBuffer := u8[]
	monitorFd := int
	monitorWds := AVLMap.{int,Tuple.{!(char^)&->void,char^}}
	initMonitor := !() -> void
	{
		initedMonitor = true
		pollData.Create().0 = 2

		if $posix
		{
			monitorBuffer = new u8[4096]
			monitorFd = inotify_init()
			setData := ref posixPollArr.Create()
			setData.fd = monitorFd
			setData.events = 0x01
			newPoll := ref pollData.Create()
			newPoll.0 = 2
		}
	}
	checkMonitor := !() -> void
	{
		readRes := read(monitorFd,monitorBuffer->{void^},4096)
		if readRes > 0
		{
			asStruct := monitorBuffer->{inotify_event^}
			if monitorWds.Contain(asStruct.wd)
			{
				nowMon := ref monitorWds[asStruct.wd]
				itLambd := nowMon.0
				newName := (""sbt + nowMon.1 + "/" + asStruct.name[0]&)->{char^}
				itLambd(newName)
			}
		}
	}
	monitorsToAdd := List.{Tuple.{char^,!(char^)&->void,bool}} ; $keep
	Monitor := !(char^ pathName,bool recursive,!(char^)&-> void callb) -> void
	{
		itMutex.Lock()
		monitorsToAdd.Emplace(pathName,callb,recursive)
		itMutex.Unlock()
		notifyMain()
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
		itMutex.Lock()
		newObj := ref sleepTasks.CreateBeforeIf(_1.0 < nextTime)
		newObj.0 = nextTime
		newObj.1 = CurrentTask
		itMutex.Unlock()
		switchToMain()
	}
	ExpectWorkers := !(int expects) -> void
	{
		diffWorks := expects - poolThread.Size()
		if diffWorks > 0
		{
			for i : diffWorks
			{
				newThread := new Thread(() ==> {
					while working
					{
						itWorkMutex.Lock()

						if itWorkToDo.Size() != 0
						{
							frstWork := itWorkToDo.Front().0
							frstTask := itWorkToDo.Front().1
							itWorkToDo.Pop()
							itWorkMutex.Unlock()
							frstWork()
							FlushTempMemory()
							itMutex.Lock()
							itWorkDone << frstTask
							notifyMain()
							itMutex.Unlock()
						}else{
							if not working
							{
								itWorkMutex.Unlock()
								return void
							}
							itWorkConVar.Wait(itWorkMutex&)
							itWorkMutex.Unlock()
						}
					}
				})
				itMutex.Lock()
				poolThread << newThread
				itMutex.Unlock()
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
		_TaskPtrReset()
		if $posix
			swapcontext(CurrentTask.uContext&,startContext)
		if $win32
			SwitchToFiber(startContext)
	}
	doTask := !(TaskData^ toRun) -> void
	{
		CurrentTask = toRun
		_TaskPtrSet(toRun.taskLocalPtr)
		if $posix
			swapcontext(startContext&,toRun.uContext&)
		if $win32
			SwitchToFiber(toRun.fiber)

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
					addMonitor(frst.0,frst.1,frst.2)
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
					toDoTask = checkDoneWorks()
				}
			}

			if toDoTask != null
			{
				itMutex.Unlock()
				doTask(toDoTask)
				continue
			}

	
			if makeWait
			{
				//itConVar.WaitFor(itMutex&,waitTime)
				itMutex.Unlock()
				if $posix
				{
					poll(posixPollArr.Data(),posixPollArr.Size(),(waitTime*1000)->{int})
					for itFd,i : posixPollArr, extra : pollData
					{
						if itFd.revents != 0
						{
						switch extra.0
						{
							case 1
								toRd := u64
								read(eventFd,toRd&,8)
							case 2
								checkMonitor()
						}
						}
					}
				}
				if $win32
				{
					res := WaitForMultipleObjects(winPollArr.Size(),winPollArr.Data(),0,(waitTime*1000)->{int})
				}
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
	checkDoneWorks := !() -> TaskData^
	{
		if itWorkDone.Size() == 0
			return null

		return itWorkDone.Pop()
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
		return startTask
	}
}
CreateTaskBox := !(int stackSize) -> TaskBox^
{
	return new TaskBox(stackSize)
}
