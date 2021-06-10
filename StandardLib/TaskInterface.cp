
TMonitor := !(char^ pathName,int modes,bool recursive,MonitorCallback callb) -> void
{
	CurrentTaskBox.Monitor(pathName,modes,recursive,callb)
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
TPauseTask := !(int^ resId) -> void
{
	if CurrentTaskBox != null
		CurrentTaskBox.PauseTask(resId)
}
TResumeTask := !(int taskId) -> void
{
	if CurrentTaskBox != null
		CurrentTaskBox.ResumeTask(taskId)
}
TExpectWorkers := !(int exp) -> void
{
	if CurrentTaskBox != null
		CurrentTaskBox.ExpectWorkers(exp)
}
TSpawnTask := !(!()&->void lmb) -> void
{
	assert(CurrentTaskBox != null)
	CurrentTaskBox.Spawn(lmb)
}

TYield := !() -> void
{
	assert(CurrentTaskBox != null)
	CurrentTaskBox.Yield()
}