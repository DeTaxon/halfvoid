
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

	if $posix
		startContext := u8[1024]
	if $win32
		startContext := void^

	itMutex := Mutex

	itWorkToDoPre := List.{Tuple.{!()&->void,TaskData^}} ; $keep

	itWorkMutex := Mutex
	itWorkConVar := ConVar
	itWorkToDo := List.{Tuple.{!()&->void,TaskData^}} ; $keep
	itWorkDone := List.{TaskData^} ; $keep
	poolThread := List.{Thread^}

	working := bool

	if $posix
		eventFd := int
	if $posix
		posixPollArr := RawArray.{pollfd}
	
	if $win32
		eventHandle := void^
	if $win32
		winPollArr := RawArray.{void^}

	pollData := RawArray.{Tuple.{int}}

	this := !() -> void
	{
		itMutex."this"()
		itWorkMutex."this"()
		itWorkConVar."this"()

		pollData."this"()
		pollData.Reserve(10)

		if $posix
		{
			posixPollArr."this"()
			posixPollArr.Reserve(10)

			eventFd = eventfd(0,0)
			setPollFd := ref posixPollArr.Create()
			setPollFd.fd = eventFd
			setPollFd.events = 0x01
			pollData.Create().0 = 1
		}

		if $win32
		{
			startContext = ConvertThreadToFiber(null)

			winPollArr."this"()
			winPollArr.Reserve(10)

			eventHandle = CreateEventA(null,0,0,null->{u8^})
			winPollArr.Create() = eventHandle
			pollData.Create().0 = 1
		}
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
	addMonitor := !(char^ pathName,!(char^)&->void callb,bool isRec) -> void
	{
		if $posix {
		if not initedMonitor initMonitor()

		pathAdd := List.{char^}() ; $temp $keep

		pathAdd << pathName

		while pathAdd.Size() != 0
		{	
			nowPath := pathAdd.Pop()

			wd := inotify_add_watch(monitorFd,nowPath,8 + 255)
			if wd >= 0
			{
				setWds := ref monitorWds[wd]
				setWds.0 = callb
				setWds.1 = StrCopy(nowPath)
				if isRec
				{
					for subFolder : Path(nowPath)
					{
						if not subFolder.IsFolder() 
							continue
						strBld := ""sbt + nowPath + "/" + subFolder.Name()
						str := strBld.Str() ; $temp
						pathAdd << str
					}
				}
			}
		}
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
		newObj := ref sleepTasks.CreateBeforeIf(x ==> x.0 < nextTime)
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
							itWorkConVar.Wait(itWorkMutex&)
							if not working
							{
								return void
							}
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
	notifyMain := !() -> void
	{
		if $posix
		{
			toWrite := u64
			toWrite = 1
			write(eventFd,toWrite&,8)
		}
		if $win32
		{
			SetEvent(eventHandle)
		}
	}
	Quit := !() -> void
	{
		working = false
		notifyMain()
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
		if first.0 < nowTime
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
		return startTask
	}
}
CreateTaskBox := !() -> TaskBox^
{
	return new TaskBox()
}
