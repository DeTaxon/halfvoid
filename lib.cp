
printf := !(char^ str, ...) -> int declare
calloc := !(int size,int sizet) -> void^ declare
malloc := !(int size) -> void^ declare
free := !(void^ point) -> void declare

dlopen := !(char^ name,int pri) -> void^ declare
dlsym := !(void^ hndl, char^ name) -> void^ declare
dlclose := !(void^ hndl) -> int declare

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
