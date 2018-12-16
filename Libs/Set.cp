#import "arrs.cp"

Set := class .{@T} extend Stack.{T}
{

	this := !() -> void
	{
		Start = null
	}
	Contain := !(T itm) -> bool
	{
		for this
			if it == itm 
				return true
		return false
	}
	Add := !(T itm) -> void
	{
		if not Contain(itm)
			Push(itm)
	}
	"==" := !(Set.{T} scnd) -> bool
	{
		if scnd.Size() != Size() return false
		for this
		{
			if not scnd.Contain(it) return false
		}
		for scnd
		{
			if not Contain(it) return false
		}

		return true
	}
	//"<=>" := !(Set.{@Y}  scnd) -> int
	//{
	//	s1 := tempItem.Size()
	//	s2 := scnd.tempItem.Size()
	//	if s1 != s2
	//	{
	//		if s1 < s2 return 1
	//		return -1
	//	}
	//}
	
}
