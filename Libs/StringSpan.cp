strlen := !(char^ str) -> int declare
strncmp := !(char^ a, char^ b, int size) -> int declare

StringSpan := class{
	ptr := char^
	itSize := int
	this := !() -> void {ptr = null itSize = 0}
	this := !(char^ st,int si) -> void{ptr = st itSize = si}

	this := !(char^ st, int si,range ran) -> void{
		startInd := 0
		if ran->begin < 0 {
			startInd = si + ran->begin
		}else{
			startInd = ran->begin
		}
		endInd := 0
		if ran->end <= 0 {
			endInd = si + ran->end - 1
		}else{
			endInd = startInd + ran->end - 1
		}
		ptr = st[startInd]&
		itSize = endInd - startInd + 1
	}
	Size := !() -> int { return itSize }
	"[]" := !(int ind) -> ref char { return ptr[ind] }
	"[]" := !(range ran) -> StringSpan
	{
		return StringSpan(ptr,itSize,ran)
	}
	"==" := !(char^ toCmp) -> bool {
		if toCmp == null return false
		for itSize {
			if toCmp[it] != ptr[it] return false
		}
		return toCmp[itSize] == 0
	}
	//"==" := !(StringSpan toCmp) -> bool {
	//	if toCmp.itSize != itSize return false
	//	return strncmp(toCmp.ptr,ptr,itSize) == 0
	//}
	Str := !() -> string {
		preRet := new char[itSize+1]
		preRet[itSize] = 0
		memcpy(preRet->{void^},ptr,itSize)
		return preRet
	}
}

"[]" := !(string this, range ran) -> StringSpan
{
	return StringSpan(this,strlen(this),ran)
}

