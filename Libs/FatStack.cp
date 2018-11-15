FatStack := class .{@DATA,@SIZE}
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
	//"[]" := !(int ind) -> ref DATA
	//{
	//	return itData[ind]
	//}
}
