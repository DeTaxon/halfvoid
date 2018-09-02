
printf := !(char^ str, ...) -> int declare
calloc := !(int size,int sizet) -> void^ declare
malloc := !(int size) -> void^ declare
free := !(void^ point) -> void declare

dlopen := !(char^ name,int pri) -> void^ declare
dlsym := !(void^ hndl, char^ name) -> void^ declare
dlclose := !(void^ hndl) -> int declare

tanf := !(float iiin) ->float declare
cosf := !(float iiin) ->float declare
sinf := !(float iiin) ->float declare
sqrtf := !(float iiin) ->float declare

"new" := !() . {@R} -> void^ 
{
	return malloc(R->TypeSize)
}
"delete" := !(void^ item) .{@R[]} -> void
{
	sub := 4
	//if R->Align > 4 sub = R->Align
	free(item - sub)
}
"~this" := !(@R[] this) -> void
{
	for this
	{
		it."~this"()
	}
}
"~this" := !(@R^ this) -> void
{
	this^."~this"()
}
"~this" := !(@R this) -> void
{
}
"delete" := !(void^ item) .{@R} -> void
{
	free(item)
}

"in" := !(int a, range b) -> bool
{
	if a < b->begin return false
	if a > b->end return false
	return true
}

IntIter := class 
{
	start,end := int
	this := !(int a,int b) -> void
	{	
		start = a
		end = b
	}
	"^" := !() -> int
	{
		return start
	}
	Inc := !() -> void
	{
		start += 1
	}
	IsEnd := !() -> bool
	{
		return start >= end
	}
	//IsInvalid := !() -> bool
	//{
	//	return false
	//}
}
"~For" := !(int x) -> IntIter
{
	return IntIter(0,x)
}
"~For" := !(range x) -> IntIter
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
	"^" := !() -> ref Type	{ return ptr[nowPos] }
	Inc := !() -> void	{ nowPos += 1 }
	IsEnd := !() -> bool	{ return nowPos >= size }
}

"~For" := !(@Type[@Size] item) -> FixedArrayIter.{Type}
{
	return FixedArrayIter.{Type}(item->{Type^},Size)
}
"~For" := !(@Type[] item) -> FixedArrayIter.{Type}
{
	return FixedArrayIter.{Type}(item->{Type^},item->len)
}
"in" := !(int val, string str) -> bool
{
	if str == null return false
	i := 0
	while str[i] != 0 {
		if val == str[i] return true
		i += 1
	}
	return false
}


