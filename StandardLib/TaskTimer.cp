AppendClass TaskBox
{
	sleepTasks := List.{Tuple.{double,TaskData^}} ; $keep
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
}