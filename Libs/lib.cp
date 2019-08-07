printf := !(char^ str, ...) -> int declare
calloc := !(size_t size,int sizet) -> void^ declare
malloc := !(size_t size) -> void^ declare
free := !(void^ point) -> void declare

tanf := !(float iiin) ->float declare
cosf := !(float iiin) ->float declare
sinf := !(float iiin) ->float declare
sqrtf := !(float iiin) ->float declare

time := !() -> int declare
srand := !(int time) -> void declare
rand := !() -> int declare

memset := !(void^ dst, char val,int size) -> void declare
memcpy := !(void^ dst, void^ src,int size) -> void declare

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
"in" := !(int val, char^ str) -> bool
{
	if str == null return false
	i := 0
	while str[i] != 0 {
		if val == str[i] return true
		i += 1
	}
	return false
}

"in" := !(SArr val, @SArr[@SSize] itArr) -> bool
{
	for i : SSize
	{
		if val == itArr[i] return true
	}
	return false
}
"in" := !(SArr val, @SArr[] itArr) -> bool
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

min := !(@A a, @B b) 
{
	if a > b return b
	return a
}
max := !(@A a, @B b) 
{
	if a > b return a
	return b
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

