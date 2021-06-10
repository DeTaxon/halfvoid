AppendClass TaskBox
{
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
							tasksToExe << frstTask
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
		ExpectWorkers(1)
		itMutex.Lock()
		itWorkCount += 1
		itWorkToDoPre.Emplace(lambd,CurrentTask)
		itMutex.Unlock()
		switchToMain()
	}
}