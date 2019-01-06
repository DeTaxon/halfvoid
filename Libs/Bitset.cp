
BitsetIterator := class
{
	dataPointer := u8^
	lowIter,bigIter := int
	bigSize := int

	this := !(u8^ val, int siz) -> void
	{
		lowIter = 0
		bigIter = 0
		bigSize = siz
		dataPointer = val
		
		while val[bigIter] == 0 and bigIter < siz
		{
			bigIter++
		}
		if bigIter < siz
		{	
			itVal := val[bigIter]
			while (itVal and_b ( 1 << lowIter)) == 0
			{
				lowIter++
			}
		}
	}
	"^" := !() -> int
	{
		return lowIter + bigIter*8
	}
	Inc := !() -> void
	{
		lowIter++
		if lowIter >= 8{
			bigIter++
			lowIter = 0
		}
		if bigIter >= bigSize return void
		while dataPointer[bigIter] < (1 << lowIter)
		{
			bigIter++
			lowIter = 0
			if bigIter >= bigSize return void
		}
		while (dataPointer[bigIter] and_b (1 << lowIter)) == 0
		{
			lowIter++
		}
	}
	IsEnd := !() -> bool 
	{	
		return bigIter >= bigSize 
	}
}


Bitset := class .{@ArraySize}
{
	itArray := u8[ArraySize]
	this := !() -> void
	{
		for itArray it = 0
	}
	"~For" := !() -> BitsetIterator
	{
		return BitsetIterator(itArray->{u8^},ArraySize)
	}
	Inverse := !() -> void
	{
		for itArray it = it xor_b 0xFF
	}
	"[]" := !(int x) -> bool
	{
		return Contain(x)
	}
	Contain := !(int x) -> bool
	{
		if x < ArraySize*8
		{
			return (itArray[x div 8] and_b (1 << (x %8))) != 0
		}
		return false
	}
	"<<" := !(int x) -> ref Bitset.{ArraySize}
	{
		if x < ArraySize*8
		{
			itArray[x div 8] = itArray[x div 8] or_b (1 << (x % 8))
		}
		return this
	}
	"<<" := !(range x) -> ref Bitset.{ArraySize}
	{
		for x this << it
		return this
	}
	Clean := !(int x) -> void
	{
		if x >= ArraySize*8 return void

		ind := x div 8
		atVal := x % 8

		itArray[ind] = itArray[ind] and_b (not_b (1 << atVal))
	}
	Clean := !() -> void
	{
		for itArray it = 0
	}

}
