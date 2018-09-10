#import "arrs.cp"

Set := class .{@T}
{
	tempItem := Stack.{T}

	Contain := !(T itm) -> bool
	{
		for tempItem
			if it == itm 
				return true
		return false
	}
	Push := !(T itm) -> void
	{
		if not Contain(itm)
			tempItem.Push(itm)
	}
	"==" := !(Set.{@Y} scnd) -> bool
	{
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
}
