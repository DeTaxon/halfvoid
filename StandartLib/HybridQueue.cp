HybridQueueIterator := class .{@T,@ExtraSize}
{
	itPtr := HybridQueue.{T,ExtraSize}^
	itIntInd := int
	itPtrItr := ListNode.{T}^
	this := !(HybridQueue.{T,ExtraSize}^ frm) -> void
	{
		itPtr = frm
		itIntInd = 0
		itPtrItr = frm.startNode
	}
	"^" := !() -> ref T
	{
		if itIntInd < ExtraSize
		{
			return itPtr.qArr[itIntInd] 
		}
		return itPtrItr.Data
	}
	IsEnd := !() -> bool { return itIntInd >= itPtr.itSize}
	Inc := !() -> void
	{
		if itIntInd >= ExtraSize
		{
			itPtrItr = itPtrItr.Next
		}
		itIntInd += 1
	}
}

HybridQueue := class .{@T,@ExtraSize}
{
	qArr := T[ExtraSize]
	startNode,endNode := ListNode.{T}^
	itSize := int
	if $keep createdNodes := ListNode.{T}^
	
	this := !() -> void
	{
		itSize = 0
		if $keep createdNodes = null
	}
	"~For" := !() -> HybridQueueIterator.{T,ExtraSize}
	{
		return HybridQueueIterator.{T,ExtraSize}(this&)
	}
	makeNewItem := !() -> ref T
	{
		if itSize < ExtraSize
		{
			itSize += 1
			return qArr[itSize - 1]
		}
		newNode := ListNode.{T}^()
		if $keep
		{
			if createdNodes != null
			{
				newNode = createdNodes
				createdNodes = createdNodes.Next
			}
		}
		if newNode == null newNode = new ListNode.{T}()
		newNode.Next = null
		if itSize == ExtraSize
		{
			startNode = newNode
			endNode = startNode
		}else{
			endNode.Next = newNode
			endNode = endNode.Next
		}
		itSize += 1
		return endNode.Data
	}
	Push := !(T itm) .{} -> void
	{
		makeNewItem() = itm
	}
	Emplace := !(args...) .{} -> void
	{
		makeNewItem()."this"(args...)
	}
	"[]" := !(int ps) -> ref T
	{
		if ps < ExtraSize
		{
			return qArr[ps]
		}
		itr := startNode
		ind := ps - ExtraSize
		while ind != 0
		{
			itr = itr.Next
			ind -= 1
		}
		return itr.Data
	}
	Clear := !() . {} -> void
	{
		if $keep
		{
			if itSize > ExtraSize
			{
				endNode.Next = createdNodes
				createdNodes = startNode
			}
			itSize = 0
		}else{
			Destroy()
		}
	}
	Destroy := !() .{} -> void
	{
		if not $temp
		{
			if $keep
			{
				while createdNodes != null
				{
					itNd := createdNodes.Next
					createdNodes = createdNodes.Next
					delete itNd
				}
			}
			if itSize > ExtraSize
			{
				while start != null
				{
					itNd := startNode
					startNode = startNode.Next
					delete itNd
				}
			}
		}
		itSize = 0
	}
	Size := !() -> int
	{
		return itSize
	}
}
