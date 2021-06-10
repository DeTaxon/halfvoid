TaskBoxWorkData := class
{
	lambda := !()&->void
	taskData := TaskData^

	this := !(!()&->void l, TaskData^ t) -> void
	{
		lambda = l
		taskData = t
	}
	"=" := default
}

AppendClass TaskBox
{
	itWorkMutex := Mutex
	itWorkConVar := ConVar
	itWorkToDo := List.{TaskBoxWorkData} ; $keep
	itWorkCount := int
	poolThread := List.{Thread^}

	ExpectWorkers := !(int expects) -> void
	{
		diffWorks := expects - poolThread.Size()
		if diffWorks <= 0
			return void
			
		for i : diffWorks
		{
			newThread := new Thread(() ==> {
				while working
				{
					itWorkMutex.Lock()

					if itWorkToDo.Size() != 0
					{
						work := itWorkToDo.Front()
						itWorkToDo.Pop()
						itWorkMutex.Unlock()
						work.lambda()
						FlushTempMemory()
						itMutex.Lock()
						tasksToExe << work.taskData
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
	AwaitWork := !(!()&->void lambd) -> void
	{
		ExpectWorkers(1)
		itWorkMutex.Lock()
		itWorkCount += 1
		itWorkToDo.Emplace(lambd,CurrentTask)
		itWorkConVar.NotifyAll()
		itWorkMutex.Unlock()
		switchToMain()
	}
	checkExeWorks := !() -> TaskData^
	{
		if tasksToExe.Size() == 0
			return null

		return tasksToExe.Pop()
	}
}