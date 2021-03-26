strlen := !(char^ str) -> int declare
strncmp := !(char^ a, char^ b, int size) -> int declare

StringSpan := class{
	ptr := char^
	itSize := int
	this := !() -> void {ptr = null itSize = 0}
	this := !(StringSpan cpy) -> void {ptr = cpy.ptr itSize = cpy.itSize}
	this := !(char^ st) -> void{ptr = st itSize = StrSize(st)}
	this := !(char^ st,int si) -> void{ptr = st itSize = si}
	this := !(char[@Size] st) -> void { ptr = st	itSize = Size }

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
	Get := !() -> char^ { return ptr }
	Size := !() -> int { return itSize }
	"[]" := !(int ind) -> ref char { return ptr[ind] }
	"[]" := !(range ran) -> StringSpan
	{
		return StringSpan(ptr,itSize,ran)
	}
	"=" := !(char^ toGet) -> ref StringSpan
	{
		this = StringSpan(toGet)
		return this
	}
	"=" := !(char[@Size] toGet) -> ref StringSpan
	{
		this = StringSpan(toGet)
		return this
	}
	"=" := !(StringSpan toGet) -> ref StringSpan
	{
		ptr = toGet.ptr
		itSize = toGet.itSize
		return this
	}
	"<=>" := !(char[@Size] st) -> int
	{
		return this <=> StringSpan(st)
	}
	"<=>" := !(char^ toCmp) -> int {
		if toCmp == null return 1
		stCmp := strncmp(ptr,toCmp,itSize)
		if stCmp != 0 return stCmp
		if toCmp[itSize] != 0 return -1
		return 0
	}
	"<=>" := !(StringSpan toCmp) -> int {
		if toCmp.ptr == null return 1
		minV := itSize
		if minV > toCmp.itSize minV = toCmp.itSize
		stCmp := strncmp(ptr,toCmp.ptr,minV)
		if stCmp != 0 return stCmp
		return itSize <=> toCmp.itSize		
	}
	"==" := !(char[@Size] st) -> bool { return this <=> st == 0 }
	"!=" := !(char[@Size] st) -> bool { return this <=> st != 0 }
	"==" := !(char^ st) -> bool { return this <=> st == 0 }
	"!=" := !(char^ st) -> bool { return this <=> st != 0 }
	"==" := !(StringSpan st) -> bool { return this <=> st == 0 }
	"!=" := !(StringSpan st) -> bool { return this <=> st != 0 }
	"<<" := !(StringSpan toGet) -> ref StringSpan
	{
		minVal := itSize
		if toGet.itSize < minVal minVal = toGet.itSize
		memcpy(ptr,toGet.ptr,minVal)
		return this
	}
	StrTmp := !()  -> char^ {
		preRet := new char[itSize+1] ; $temp
		preRet[itSize] = 0
		memcpy(preRet->{void^},ptr,itSize)
		return preRet
	}
	Str := !()  -> char^ { //TODO: put back .{}
		//preRet := new char[itSize+1]
		preRet := malloc(itSize+1)->{char^}
		preRet[itSize] = 0
		memcpy(preRet->{void^},ptr,itSize)
		return preRet
	}
	"->{}" := !() .{char^}-> char^
	{
		return Str() ; $temp
	}
	DivideStr := !(char^ to) -> List.{StringSpan}
	{
		result."this"()
		i := 0
		j := 0
		while ptr[j] != 0 and j != itSize
		{
			if ptr[j] in to
			{
				if j != i result.Emplace(ptr[i]&,j - i) ; $temp
				i = j+1
				j += 1
			} else
			{
				j += 1
			}
		}
		if j - i != 0
		{
			result.Emplace(ptr[i]&,j - i) ; $temp
		}		
	}
}

"[]" := !(string this, range ran) -> StringSpan
{
	return StringSpan(this,strlen(this),ran)
}

StrToInt := !(StringSpan strSpn) -> int 
{
	Sum := 0
	i := 0
	Min := false
	a := strSpn.ptr
	if ( a[0] == '-')
	{
		i = 1
		Min = true
	}
	while a[i] != 0 and a[i] <= '9' and a[i] >= '0' and i < strSpn.itSize
	{
		Sum *= 10
		Sum += a[i] - '0'
		i += 1	
	}
	if Min return -Sum
	return Sum
}