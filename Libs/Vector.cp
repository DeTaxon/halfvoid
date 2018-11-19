#import "lib.cp"

Vector := class .{@T}
{
	itPointer := T[]
	itSize,itCapacity := int
	this := !() -> void
	{
		itPointer = null
		itSize = 0
		itCapacity = 0
	}
	this := !(int siz) -> void
	{
		itPointer = new T[siz]
		itSize = siz
		itCapacity = siz
	}
	"[]" := !(int ind) -> ref T
	{
		return itPointer[ind]
	}
	Size := !() -> int
	{
		return itSize
	}
	"~For" := !() -> FixedArrayIter.{T}
	{
		return FixedArrayIter.{T}(itPointer->{T^},itSize)
	}
	"~this" := !() -> void
	{
		if itPointer != null
		{
			delete itPointer
		}
	}
}
