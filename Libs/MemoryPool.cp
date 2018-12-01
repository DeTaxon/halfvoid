


StupidMemoryPool := class .{@PageSize}
{
	itPage := u8^
	itLoaded := int

	this := !() -> void
	{
		itPage = null
		itLoaded = 0
	}
	GetMem := !(int size, int align) -> void^
	{
		if itPage == null{
			itPage = malloc(PageSize)
			memset(itPage,0,PageSize)
		}
		newSize := itLoaded + size
		if newSize % align != 0 {
			newSize += align
		}
		if newSize > PageSize
		{
			itPage = malloc(PageSize)
			memset(itPage,0,PageSize)
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
}

