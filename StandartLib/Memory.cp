gTemporaryPool := thread_local StupidMemoryPool.{16000000}
FlushTempMemory := !() -> void
{
	gTemporaryPool.FlushMem()
}

IMemoryPool := class
{
	GetMem := virtual !(size_t size, int align,int prefix) -> void^
	{
	}
	FreeMem := virtual !(void^ memPtr) -> void
	{
	}
	Push := !() -> void
	{
		hUserPoolStack[hUserPoolCount]  = hUserPoolCurrentPool
		hUserPoolCount += 1
		hUserPoolCurrentPool = this&
	}
	Pop := !() -> void
	{
		hUserPoolCount -= 1 
		hUserPoolCurrentPool = hUserPoolStack[hUserPoolCount]
	}
}

gMallocTemporary := !(size_t nSize, size_t nAlign) -> void^
{
	return gTemporaryPool.GetMem(nSize,nAlign)
}

prvtMemInfo := class
{
	nextBlock := prvtMemInfo^
	counter := int
	pointers := void^[256]
	realPointers := void^[256]
}

prvtPtrArray := prvtMemInfo^[2048]
prvtPtrArrayMutexes := AtomicMutex[2048]

prvtGetBlockId := !(void^ toPt) -> int
{
	someVal := toPt
	return (someVal&->{size_t^}^ >> 4) % 2048
}

hUserPoolStack := thread_local IMemoryPool^[64]
hUserPoolCurrentPool := thread_local IMemoryPool^
hUserPoolCount := thread_local int

"new" := !() . {@R} -> void^ 
{
	val := R->TypeSize
	if $temp {
		return gTemporaryPool.GetMem(val,R->Align,0)
	}
	usePool := hUserPoolCount != 0
	if $no_pool{
		usePool = false
	}
	if usePool {
		return hUserPoolCurrentPool.GetMem(val,R->Align,0)
	}
	newNode :=  malloc(val)
	memset(newNode,0,val)
	return newNode
}
"new" := !(int count) .{@R} -> R[]
{
	val := R->FatTypeSize
	itSi := val*count
	itAl := R->Align
	if itAl < 4 itAl = 4
	itSi += itAl + 16
	newNode := null->{u8^}
	if $temp {
		newNode = gTemporaryPool.GetMem(itSi,itAl,0)->{u8^}
	}else{
		usePool := hUserPoolCount != 0
		if $no_pool{
			usePool = false
		}
		if usePool{
			newNode = hUserPoolCurrentPool.GetMem(itSi,itAl,0)->{u8^}
		}else{
			newNode = malloc(itSi)->{u8^}
			preRet2 := newNode[itAl]&->{R[]}
			itId := prvtGetBlockId(preRet2->{void^})
			prvtPtrArrayMutexes[itId].Lock()
			toPut := prvtPtrArray[itId]
			if toPut == null
			{
				preSet := malloc(prvtMemInfo->TypeSize)->{prvtMemInfo^}
				memset(preSet,0,prvtMemInfo->TypeSize)
				toPut = preSet
				prvtPtrArray[itId] = preSet
			}else{
				while toPut.counter == 256 and toPut.nextBlock != null
					toPut = toPut.nextBlock
				if toPut.counter == 256
				{
					preSet := malloc(prvtMemInfo->TypeSize)->{prvtMemInfo^}
					memset(preSet,0,prvtMemInfo->TypeSize)
					toPut.nextBlock = preSet
					toPut = preSet
				}
			}
			toPut.pointers[toPut.counter] = preRet2->{void^}
			toPut.realPointers[toPut.counter] = newNode
			toPut.counter++
			prvtPtrArrayMutexes[itId].Unlock()
			memset(newNode,0,itSi)
		}
	}
	preRet := newNode[itAl]&->{R[]}
	preRet->{int^}[-1] = count
	return preRet
}
"delete" := !(void^ item) -> void
{
	itId := prvtGetBlockId(item)
	toCheck := prvtPtrArray[itId]
	found := false
	foundId := 0
	while toCheck != null
	{
		for i : toCheck.counter
		{
			if toCheck.pointers[i] == item
			{
				found = true
				foundId = i
				break
			}
		}
		if found
			break
		toCheck = toCheck.nextBlock
	}
	if found
	{
		free(toCheck.realPointers[foundId])
		prvtPtrArrayMutexes[itId].Lock()
		lastId := toCheck.counter - 1
		toCheck.pointers[foundId] = toCheck.pointers[lastId]
		toCheck.realPointers[foundId] = toCheck.realPointers[lastId]
		toCheck.counter--
		prvtPtrArrayMutexes[itId].Unlock()
	}else{
		free(item)
	}
}
