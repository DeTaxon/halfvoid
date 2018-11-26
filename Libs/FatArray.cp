FatArrayIterator := class .{@DATA}
{
	itPtr := DATA^
	itInd,itSize := int
	this := !(DATA^ itBegin, int inpSize) -> void
	{
		itPtr = itBegin
		itInd = 0
		itSize = inpSize
	}
	Inc := !() -> void { itInd += 1}
	"^" := !() -> ref DATA { return itPtr[itInd] }
	IsEnd := !() -> bool {	return itInd >= itSize 	} 
}

FatArray := class .{@DATA,@SIZE}
{
	itData := DATA[SIZE]
	itSize := int
	this := !() -> void
	{
		itSize = 0
	}
	Push := !(DATA dt) -> void
	{
		itData[itSize] = dt
		itSize++
	}
	Pop := !() -> DATA
	{
		itSize--
		return itData[itSize]
	}
	IsEmpty := !() -> bool
	{	
		return itSize == 0
	}
	Back := !() -> ref DATA { return itData[itSize - 1] }
	Size := !() -> int { return itSize }
	"[]" := !(int ind) -> ref DATA
	{
		return itData[ind]
	}
	Emplace := !() -> void
	{
		itSize += 1
	}
	Emplace := !(a) -> void
	{
		itData[itSize]."this"(a)
		itSize += 1
	}	
	Emplace := !(a,b) -> void
	{
		itData[itSize]."this"(a,b)
		itSize += 1
	}
	"~For" := !() -> FatArrayIterator.{DATA}
	{
		return FatArrayIterator.{DATA}(itData[0]&,itSize)
	}
}
