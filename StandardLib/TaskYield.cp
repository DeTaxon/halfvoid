AppendClass TaskBox
{
	yieldedTasks := List.{TaskData^} ; $keep

	Yield := !() -> void
	{
		yieldedTasks.Push(CurrentTask)
		switchToMain()
	}
	checkYields := !() -> TaskData^
	{
		if yieldedTasks.Size() == 0
			return null
		return yieldedTasks.Pop()
	}
}