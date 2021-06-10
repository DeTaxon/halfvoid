AppendClass TaskBox
{
	monitorsToAdd := List.{Tuple.{char^,MonitorCallback,int,bool}} ; $keep
	Monitor := !(char^ pathName,int modes,bool recursive,MonitorCallback callb) -> void
	{
		itMutex.Lock()
		monitorsToAdd.Emplace(pathName,callb,modes,recursive)
		itMutex.Unlock()
		notifyMain()
	}
	monitorPushWork := !() -> void
	{			
		while monitorsToAdd.Size() != 0
		{
			frst := ref monitorsToAdd.Front()
			addMonitor(frst.0,frst.1,frst.2,frst.3)
			monitorsToAdd.Pop()
		}
	}
}