#import "arrs.cp"

Set := class .{@T}
{
	tempItem := Stack.{T}

	this := !() -> void
	{
		tempItem.Start = null
	}
	Contain := !(T itm) -> bool
	{
		for tempItem
			if it == itm 
				return true
		return false
	}
	Add := !(T itm) -> void
	{
		if not Contain(itm)
			tempItem.Push(itm)
	}
	"==" := !(Set.{@Y} scnd) -> bool
	{
		if scnd.tempItem.Size() != tempItem.Size() return false
		for tempItem
		{
			if not scnd.Contain(it) return false
		}
		for scnd.tempItem
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
