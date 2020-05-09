AtomicMutex := class
{
	atmValue := u8
	Lock := !() -> void
	{
		while InterlockSet(atmValue,1) != 0 {}
	}
	Unlock := !() -> void
	{
		InterlockSet(atmValue,0)
	}
}

MutexCondPoolType := class
{
	freedMtx := List.{Mutex^} ; $keep $no_pool
	mutexMutex := AtomicMutex

	GetMutex := !() -> Mutex^
	{
		if freedMtx.Size() == 0 {
			return new Mutex() $no_pool
		}
		mutexMutex.Lock()
		defer mutexMutex.Unlock()
		if freedMtx.Size() == 0 {
			return new Mutex() $no_pool
		}
		return freedMtx.Pop()
	}
	Release := !(Mutex^ toAdd) -> void
	{
		miniMutex.Lock()
		defer miniMutex.Unlock()
		freedMtx << toAdd
	}

	freedCond := List.{ConVar^} ; $keep $no_pool
	condMutex := AtomicMutex
	GetConVar := !() -> ConVar^
	{
		if freedCond.Size() == 0 {
			return new ConVar() $no_pool
		}
		condMutex.Lock()
		defer condMutex.Unlock()
		if freedCond.Size() == 0 {
			return new ConVar() $no_pool
		}
		return freedCond.Pop()
	}
	Release := !(ConVar^ toAdd) -> void
	{
		condMutex.Lock()
		defer condMutex.Unlock()
		freedCond << toAdd
	}
}
TLockPool := MutexCondPoolType

GetPointerHash13n8 := !(void^ val) -> int
{
	itF := u64
	itF&->{void^^}^ = val
	res := 0
	for 8
	{
		res += itF % 13
		itF = itF div 13
	}
	return res % 13
}

internalLockMiniMutex := AtomicMutex
internalLockMutex := Mutex^[13] 
internalLockUsing := RBMap.{void^,Tuple.{ConVar^,int,int}}[13] ; $keep $no_pool
TLock := !(void^ k) -> void { TLock(k,1)}
TLock := !(void^ k,int count) -> void
{
	itHash := GetPointerHash13n8(k)

	if internalLockMutex[itHash] == null
	{
		internalLockMiniMutex.Lock()
		if internalLockMutex[itHash] == null
			internalLockMutex[itHash] = TLockPool.GetMutex()
		internalLockMiniMutex.Unlock()
	}

	internalLockMutex[itHash].Lock()
	defer internalLockMutex[itHash].Unlock()

	if internalLockUsing[itHash].Contain(k)
	{
		itEnt := ref internalLockUsing[itHash][k]
		itEnt.1 += 1
		while itEnt.2 == 0
			itEnt.0.Wait(internalLockMutex[itHash])
		itEnt.2 -= 1
	}else{
		newEnt := ref internalLockUsing[itHash][k]
		newEnt.0 = TLockPool.GetConVar()
		newEnt.1 = 1
		newEnt.2 = count - 1
	}
}
TUnlock := !(void^ k) -> void
{
	itHash := GetPointerHash13n8(k)

	internalLockMutex[itHash].Lock()
	defer internalLockMutex[itHash].Unlock()
	
	itEnt := ref internalLockUsing[itHash][k]
	itEnt.1 -= 1
	if itEnt.1 == 0
	{
		internalLockUsing[itHash].Remove(k)
		TLockPool.Release(itEnt.0)
	}else{
		itEnt.2 += 1
		itEnt.0.Notify()
	}
}

internalBarMiniMutex := AtomicMutex
internalBarMutex := Mutex^
internalBarUsing := RBMap.{void^,Tuple.{ConVar^,int}} ; $no_pool $keep
TBarrier := !(void^ k,int count) -> void
{
	if count <= 1 
		return void
	
	internalBarMiniMutex.Lock()
	if internalBarMutex == null
		internalBarMutex = TLockPool.GetMutex()
	internalBarMiniMutex.Unlock()
	
	internalBarMutex.Lock()
	defer internalBarMutex.Unlock()

	if internalBarUsing.Contain(k)
	{
		itEnt := ref internalBarUsing[k]
		itEnt.1 -= 1
		
		if itEnt.1 == 0
		{
			itEnt.0.NotifyAll()
		}else{
			while itEnt.1 != 0
				itEnt.0.Wait(internalBarMutex)
		}
	}else{
		newEnt := ref internalBarUsing[k]
		newEnt.0 = TLockPool.GetConVar()
		newEnt.1 = count - 1

		while newEnt.1 != 0
			newEnt.0.Wait(internalBarMutex)
		TLockPool.Release(newEnt.0)
		internalBarUsing.Remove(k)
	}	
}
