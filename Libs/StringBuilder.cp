StringBuilder := class .{@BufSize}{
	ptr := char^
	inPtrSize := int
	pages := Stack.{char^}

	this := !() -> void { ptr = null inPtrSize = 0 pages."this"()}

	"+" := !(char^ adding) -> ref StringBuilder.{BufSize}
	{
		return this << adding
	}
	"<<" := !(char^ adding) -> ref StringBuilder.{BufSize}
	{
		sLen := strlen(adding)

		if sLen == 0 return this

		//TODO: remove temp here
		if ptr == null ptr = new char[BufSize] ; $temp
		
		if sLen + inPtrSize > BufSize
		{
			moved := 0
			diff := sLen + inPtrSize - BufSize
			memcpy(ptr[inPtrSize]&,adding,sLen - diff)
			moved += sLen - diff
			pages.Push(ptr)
			while sLen - moved > BufSize {
				preSet := new char[BufSize]
				memcpy(preSet[0]&,adding[moved]&,BufSize)
				pages.Push(preSet)
				moved += BufSize
			}
			ptr = new char[BufSize]
			memcpy(ptr[0]&,adding[moved]&,sLen - moved)
			inPtrSize = sLen - moved

		}else{
			memcpy(ptr[inPtrSize]&,adding,sLen)
			inPtrSize += sLen
		}

		return this
	}
	"<<" := !(int x) -> ref StringBuilder.{BufSize}
	{
		return this + x
	}
	"+" := !(int x) -> ref StringBuilder.{BufSize}
	{
		miniBuf := char[32]
		miniBuf[31] = 0
		k := 30
		si := x < 0
		if si x = -x
		if x == 0 {
			miniBuf[k] = '0'
			k -= 1
		}
		while x != 0
		{
			miniBuf[k] = x % 10
			miniBuf[k] += '0'
			k -= 1
			x = x div 10
		}
		if si {
			miniBuf[k] = '-'
			k -= 1
		}
		return this << miniBuf[k+1]&
	}
	Str := !() -> string {
		if inPtrSize == 0{
			return ""
		}
		outSize := pages.Size()*BufSize + inPtrSize

		indItr := outSize - inPtrSize
		res := new char[outSize]
		res[outSize] = 0
		memcpy(res[indItr]&,ptr,inPtrSize)
		indItr -= BufSize

		for pages
		{
			memcpy(res[indItr]&,it,BufSize)
			indItr -= BufSize
		}
		return res
	}
	"->{}" := !() .{char^} -> string
	{
		return Str()
	}
}

"sbt" := !(string toAp) -> StringBuilder.{256}
{
	ToRet."this"()
	ToRet << toAp
	return void
}
