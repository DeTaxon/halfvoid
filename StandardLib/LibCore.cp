
"delete" := !(void^ item) .{@R} -> void
{
	free(item)
}

"in" := !(range b,int a) -> bool
{
	if a < b->begin return false
	if a > b->end return false
	return true
}

IntIter := class 
{
	start,end := int
	this := !(int a,int b) .{} -> void
	{	
		if $reverse
		{
			start = b
			end = a
		}else{
			start = a
			end = b
		}
	}
	"^" := !() -> int
	{
		return start
	}
	Inc := !() .{} -> void
	{
		if $reverse
		{
			start -= 1
		}else{
			start += 1
		}
	}
	IsEnd := !() .{} -> bool
	{
		if $reverse
		{
			return start < end
		}else{
			return start >= end
		}
	}
	//IsInvalid := !() -> bool
	//{
	//	return false
	//}
}
"~For" := !(int x) .{} -> IntIter
{
	return IntIter(0,x)
}
"~For" := !(range x)  .{} -> IntIter
{
	return IntIter(x->begin,x->end + 1)
}

FixedArrayIter := class .{@Type}
{
	ptr := Type^
	size,nowPos := int
	this := !(Type^ itm, int si) -> void
	{
		ptr = itm
		size = si		
		nowPos = 0
	}
	"^" := !() .{} -> ref Type	
	{ 
		if $reverse
		{
			return ptr[size - nowPos - 1] 
		}else{
			return ptr[nowPos] 
		}
	}
	Inc := !() -> void	{ nowPos += 1 }
	IsEnd := !() -> bool	{ return nowPos >= size }
	Ind := !() -> int { return nowPos }
}

"~For" := !(@Type[@Size] item) -> FixedArrayIter.{Type}
{
	return FixedArrayIter.{Type}(item->{Type^},Size)
}
"~For" := !(@Type[] item) -> FixedArrayIter.{Type}
{
	return FixedArrayIter.{Type}(item->{Type^},item->len)
}
"in" := !(char^ str,int val) -> bool
{
	if str == null return false
	i := 0
	while str[i] != 0 {
		if val == str[i] return true
		i += 1
	}
	return false
}

"in" := !(@SArr[@SSize] itArr,SArr val) -> bool
{
	for i : SSize
	{
		if val == itArr[i] return true
	}
	return false
}
"in" := !(@SArr[] itArr,SArr val) -> bool
{
	for i : itArr->len
	{
		if val == itArr[i] return true
	}
	return false
}
IsPrintable := !(int ch) -> bool
{
	if ch in 'a'..'z' return true
	if ch in 'A'..'Z' return true
	if ch in '0'..'9' return true
	if ch in "_(){}[]!@#$%^&*~\"\\/.," return true
	return false
}

min := !(@A a, args...) -> #best(A,args) 
{
	mx := #best(A,args)
	mx = a
	(mx = args < mx ?: args : mx)...
	return mx
}
max := !(@A a, args...) -> #best(A,args) 
{
	mx := #best(A,args)
	mx = a
	(mx = args > mx ?: args : mx)...
	return mx
}

StrSize := !(string xs) -> int
{
	itSize := 0
	i := 0
	while xs[i] != 0
	{
		itSize++
		i++
	}
	return itSize
}


abs := !(@T a)  -> T
{
	if a < 0 return -a
	return a
}
