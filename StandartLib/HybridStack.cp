HybridStackIterator := class .{@T}
{
	arrPtr := T^
	itIntInd := int
	itPtrItr := ListNode.{T}^
	nodesSize := int
	minSize := int
	itSize := int
	ExtraSize := int
	this := !(T^ aPtr,ListNode.{T}^ startPtr,int iItSize,int iExtraSize) -> void
	{
		ExtraSize = iExtraSize
		itPtrItr = startPtr
		arrPtr = aPtr
		itIntInd = 0
		itSize = iItSize
		nodesSize = itSize - ExtraSize
		minSize = itSize
		if minSize > ExtraSize
			minSize = ExtraSize
	}
	"^" := !() -> ref T
	{
		if itIntInd >= nodesSize
		{
			return arrPtr[minSize - (itIntInd - nodesSize) - 1] 
		}
		return itPtrItr.Data
	}
	IsEnd := !() -> bool { return itIntInd >= itSize}
	Inc := !() -> void
	{
		if itIntInd < nodesSize
		{
			itPtrItr = itPtrItr.Next
		}
		itIntInd += 1
	}
}

HybridStack := class .{@T,@ExtraSize}
{
	qArr := T[ExtraSize]
	startNode := ListNode.{T}^
	itSize := int

	if $keep createdNodes := ListNode.{T}^
	
	this := !() -> void
	{
		itSize = 0
		startNode = null
		if $keep createdNodes = null
	}
	"~For" := !() -> HybridStackIterator.{T}
	{
		return HybridStackIterator.{T}(qArr[0]&,startNode,itSize,ExtraSize)
	}
	makeNewItem := !() .{} -> ref T
	{
		itSize += 1
		if itSize <= ExtraSize
		{
			return qArr[itSize - 1]
		}
		if $keep
		{
			if createdNodes != null
			{
				newNode := createdNodes
				createdNodes = createdNodes.Next
				newNode.Next = startNode
				startNode = newNode
				return newNode.Data
			}
		}
		oldVal := startNode
		startNode = new ListNode.{T}()
		startNode.Next = oldVal
		return startNode.Data
	}
	Push := !(T itm) .{} -> void
	{
		makeNewItem() = itm
	}
	"<<" := !(T itm) .{} self_return
	{
		makeNewItem() = itm
	}
	Emplace := !(args...) -> void
	{
		makeNewItem()."this"(args...)
	}
	"[]" := !(int ps) -> ref T
	{
		nodesCount := itSize - ExtraSize
		if ps >= nodesCount
		{
			minSize := itSize
			if minSize > ExtraSize
				minSize = ExtraSize
			return qArr[minSize - (ps - nodesCount) - 1]
		}
		itr := startNode
		ind := ps
		while ind != 0
		{
			itr = itr.Next
			ind -= 1
		}
		return itr.Data
	}
	Size := !() -> int
	{
		return itSize
	}
	Clear := !() .{} -> void
	{
		if $keep
		{
			if startNode != null
			{
				itr := startNode
				while itr.Next != null itr = itr.Next
				itr.Next = createdNodes
				createdNodes = startNode
				startNode = null
			}
		}else{
			Destroy()
		}
		itSize = 0
	}
	Destroy := !() .{}-> void
	{
		if not $temp
		{
			if itSize > ExtraSize
			{
				while startNode != null
				{
					itNode := startNode
					startNode = startNode.Next
					delete itNode
				}
			}
			if $keep
			{
				while createdNodes != null
				{
					itNode := createdNodes
					createdNodes = createdNodes.Next
					delete itNode
				}
			}
		}
		itSize = 0
	}
}
