TEchoStream := class extend TIOStream
{
	ptr := char^
	inPtrSize := int
	pages := List.{char^}

	this := !() -> void { ptr = null inPtrSize = 0 pages."this"()}
	Read := virtual !(void^ data, size_t size) -> size_t
	{
		return 0 //TODO implement
	}
	Write := virtual !(void^ adding, size_t sLen) -> size_t
	{
		if sLen == 0 return 0
		
		BufSize := 4096
		if ptr == null ptr = new char[BufSize]
		if sLen + inPtrSize > BufSize
		{
			moved := 0
			diff := sLen + inPtrSize - BufSize
			memcpy(ptr[inPtrSize]&,adding,sLen - diff)
			moved += sLen - diff
			
			pages.Push(ptr)
			while sLen - moved > BufSize {
				preSet := new char[BufSize]
				memcpy(preSet[0]&,adding->{char^}[moved]&,BufSize)
				pages << preSet
				moved += BufSize
			}
			ptr = new char[BufSize]
			memcpy(ptr[0]&,adding->{char^}[moved]&,sLen - moved)
			inPtrSize = sLen - moved

		}else{
			memcpy(ptr[inPtrSize]&,adding,sLen)
			inPtrSize += sLen
		}
		return sLen
	}
	Str := !()  -> char^ {
		if inPtrSize == 0{
			return ""
		}
		BufSize := 4096
		indItr := pages.Size()*BufSize
		outSize := indItr + inPtrSize

		res := new char[outSize + 1]
		res[outSize] = 0
		memcpy(res[indItr]&,ptr,inPtrSize)
		//indItr -= BufSize
		indItr = 0

		for pages
		{
			memcpy(res[indItr]&,it,BufSize)
			indItr += BufSize
		}
		return res
	}
	Close := virtual !() -> void
	{
		//TODO: mem clean up
	}
}
