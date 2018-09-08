
BitsetIterator := class
{
	dataPointer := u8^
	lowIter,bigIter := int
	bigSize := int

	oldBool,nowBool := bool

	this := !(u8^ val, int siz) -> void
	{
		lowIter = 0
		bigIter = 0
		bigSize = siz
		dataPointer = val
	}
	"^" := !() -> ref bool
	{
		nowBool = ((dataPointer[bigIter] and_b (1 << lowIter)) != 0)
		oldBool = nowBool
		return nowBool
	}
	Inc := !() -> void
	{
		if nowBool != oldBool
		{
			mask := 1 << lowIter
			dataPointer[bigIter] = dataPointer[bigIter] xor_b mask
		}
		lowIter++
		if lowIter >= 8
		{
			lowIter = 0
			bigIter++
		}
	}
	Ind := !() -> int { return bigIter*8 + lowIter }
	IsEnd := !() -> bool {	return bigIter >= bigSize }
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

}
