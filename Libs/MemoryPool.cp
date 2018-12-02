
StupidMemoryPool := class .{@PageSize}
{
	itPage := u8^
	itLoaded := int

	itBusyPages := Stack.{u8^}
	itFreedPages := Stack.{u8^}

	this := !() -> void
	{
		itPage = null
		itLoaded = 0
		itBusyPages."this"()
		itFreedPages."this"()
	}
	GetPage := !() -> u8^
	{	
		newPage := null->{u8^}
		if itFreedPages.Empty()
		{
			newPage = malloc(PageSize)
		}else{
			newPage = itFreedPages.Pop()
		}
		memset(newPage,0,PageSize)
		return newPage
	}
	GetMem := !(int size, int align) -> void^
	{
		if itPage == null{
			itPage = GetPage()
		}
		newSize := itLoaded + size
		if newSize % align != 0 {
			newSize += align
		}
		if newSize > PageSize
		{
			itBusyPages.Push(itPage)
			itPage = GetPage()
			itLoaded = 0
			return itPage
		}
		offs := newSize - size
		itLoaded = newSize
		return itPage[offs]&
	}
	FreeMem := !(void^ itm) -> void
	{
	}
	FlushMem := !() -> void
	{
		if itPage != null {
			memset(itPage,0,itLoaded)
		}
		itLoaded = 0
		itFreedPages <<< itBusyPages
	}
}

