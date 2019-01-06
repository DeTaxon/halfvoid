

MutexPool := class
{
	boxMutex := Mutex
	freeMutexes := Stack.{Mutex^}

	this := !() -> void
	{
		boxMutex."this"()
	}

	GetMutex := !() -> Mutex^
	{
		
	}
	ReleaseMutex := !() -> void
	{
	}
	Close := !() -> void
	{	
		boxMutex.Close()
		freeMutexes[^].Close()
	}
}

gMutexPool := MutexPool

FastMutex := class
{
	miniMutex := int
	partySize := int
	bigMutex := Mutex^
	Lock := !() -> void
	{
		prevValue := InterlockAdd(miniMutex,1)
		if prevValue != 0{
			if prevValue == 1{
				while bigMutex != null {}
				bigMutex = gMutexPool.GetMutex()
			}else{
				while bigMutex == null {}
			}
			InterlockAdd(partySize)
			bigMutex.Lock()
		}
	}
	Unlock := !() -> void
	{
		prevValue := InterlockSub(miniMutex)
		if prevValue != 1{
			if prevValue == 2{
				while partySize != 1 {}
			}else{
			}
		}
	}
}
