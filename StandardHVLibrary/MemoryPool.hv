AllocOnlyMP := class .{@PageSize,@CleanResult} extend IMemoryPool
{
	itPage := u8^
	itLoaded := int
	createdPages := List.{void^} ; $no_pool

	GetPage := !() -> u8^
	{	
		newPage := null->{u8^}
		newPage = malloc(PageSize)
		if CleanResult
		{
			memset(newPage,0,PageSize)
		}
		createdPages << newPage ; $no_pool
		return newPage
	}
	GetMem := virtual !(size_t size, int align,int prefix) -> void^
	{
		if size > PageSize
		{
			preRe := malloc(size)
			if CleanResult memset(preRe,0,size)
			createdPages << preRe ; $no_pool
			return preRe
		}
		if itPage == null{
			itPage = GetPage()
		}
		newSize := itLoaded
		if align > 1 {
			newSize +=  0xFF % align
			newSize -= newSize % align
		}
		itLoaded = newSize + size

		if itLoaded >= PageSize
		{
			itPage = GetPage()
			itLoaded = size
			return itPage
		}
		return itPage[newSize]&
	}
	FreeMem := virtual !(void^ memPtr) -> void
	{
	}
	Clear := !() -> void
	{
		free(createdPages[^])
	}
}


StupidMemoryPool := class .{@PageSize} extend IMemoryPool
{
	itPage := u8^
	itLoaded := int

	itBusyPages := List.{u8^}
	itFreedPages := List.{u8^}

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
		if itFreedPages.IsEmpty()
		{
			newPage = malloc(PageSize)
		}else{
			newPage = itFreedPages.Pop() ; $no_pool
		}
		memset(newPage,0,PageSize)
		return newPage
	}
	GetMem := virtual !(int size, int align, int prefix) -> void^
	{
		if itPage == null{
			itPage = GetPage()
		}
		newSize := itLoaded
		if align > 1 {
			newSize +=  0xFF % align
			newSize -= newSize % align
		}
		itLoaded = newSize + size

		if itLoaded >= PageSize
		{
			itBusyPages.Push(itPage) ; $no_pool
			itPage = GetPage()
			itLoaded = size
			return itPage
		}
		return itPage[newSize]&
	}
	FreeMem := virtual !(void^ itm) -> void
	{
	}
	FlushMem := !() -> void
	{
		if itPage != null {
			memset(itPage,0,itLoaded)
		}
		itLoaded = 0
		itFreedPages <<< itBusyPages ; $no_pool
	}
}

