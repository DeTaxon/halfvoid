
M_CREATED := 1
M_DELETED := 2
M_CHANGED := 4

MonitorCallback := type !(char^,int)&-> void

AppendClass TaskBox
{
	monitorsToAdd := List.{Tuple.{char^,MonitorCallback,int,bool}} ; $keep
	Monitor := !(char^ pathName,int modes,bool recursive,MonitorCallback callb) -> void
	{
		itMutex.Lock()
		monitorsToAdd.Emplace(pathName,callb.Capture(),modes,recursive)
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