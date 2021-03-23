StringEnd := class
{
	ptrToStr := char^
	strSize := int

	this := !(char^ fromStr) -> void
	{
		ptrToStr = fromStr
		strSize = strlen(ptrToStr)
	}
	this := !(StringSpan spn) -> void
	{
		ptrToStr = spn.Get()
		strSize = spn.Size()
	}
	"<=>" := !(char^ toCmp) -> int
	{
		return this <=> StringSpan(toCmp)
	}
	"<=>" := !(StringSpan toCmp) -> int
	{
		assert(toCmp.Size() <= strSize)
		thisSpn := StringSpan(ptrToStr[strSize - toCmp.Size()]&,toCmp.Size())
		return thisSpn <=> toCmp
	}
}
