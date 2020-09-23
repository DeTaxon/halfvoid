
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
}
ucontextStartTask := !(void^ fiberData) -> void
{
	CurrentTask.tskToRun()
	CurrentTaskBox.onDestroyTask(CurrentTask)
	if $posix
		setcontext(CurrentTaskBox.startContext&)
	if $win32
		SwitchToFiber(CurrentTaskBox.startContext)
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

	this := !() -> void
	{
		itMutex."this"()
		itConVar."this"()

		if $win32
			startContext = ConvertThreadToFiber(null)
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
		if $posix
			swapcontext(CurrentTask.uContext&,startContext)
		if $win32
			SwitchToFiber(startContext)
	}
	doTask := !(TaskData^ toRun) -> void
	{
		CurrentTask = toRun
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
