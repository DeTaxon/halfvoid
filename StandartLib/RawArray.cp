RawArrayIter := class.{@T}
{
	itPtr := T^
	itSize := int
	itPos := int
	this := !(T^ ptr,int siz) -> void
	{
		itPtr = ptr
		itSize = siz
		itPos = 0
	}
	Inc := !() -> void	{ itPos += 1 }
	"^" := !() -> ref T { return itPtr[itPos] }
	"IsEnd" := !() -> bool { return itPos >= itSize }
}

RawArray := class .{@T}
{
	itPtr := T[]
	itSize := int
	this := !() -> void
	{
		itPtr = null
		itSize = 0
	}
	Data := !() -> T^ 
	{
		return itPtr
	}
	Reserve := !(int toResrv) -> void
	{
		if itPtr == null or itPtr->len < toResrv
		{
			oldPtr := itPtr
			itPtr = new T[toResrv]
			if itSize > 0
			{
				memcpy(itPtr->{void^},oldPtr->{void^},(itSize*(T->TypeSize))->{int})
			}
			if oldPtr != null
				delete oldPtr
		}
	}
	Create := !() -> ref T
	{
		if itPtr == null or itPtr->len == itSize
		{
			Reserve(itPtr == null ?: 12 : itPtr->len*2) //magic
		}
		return itPtr[itSize++]
	}
	Remove := !(int nr) -> void
	{
		assert(nr < itSize)
		assert(nr >= 0)
		if nr != itSize - 1
		{
			memcpy(itPtr[nr]&,itPtr[itSize - 1]&,T->TypeSize)
		}
		itSize -= 1
	}
	Size := !() -> int
	{
		return itSize
	}
	"~For" := !() -> RawArrayIter.{T}
	{
		return RawArrayIter.{T}(itPtr,itSize)
	}
}


