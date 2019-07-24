HybridQueueIterator := class .{@T,@ExtraSize}
{
	itPtr := HybridQueue.{T,ExtraSize}^
	itIntInd := int
	itPtrItr := Node.{T}^
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
	startNode,endNode := Node.{T}^
	itSize := int
	
	this := !() -> void
	{
		itSize = 0 
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
		if itSize == ExtraSize
		{
			startNode = new Node.{T}()
			endNode = startNode
		}else{
			endNode.Next = new Node.{T}()
			endNode = endNode.Next
		}
		itSize += 1
		return endNode.Data
	}
	Push := !(T itm) -> void
	{
		makeNewItem() = itm
	}
	Emplace := !(a) -> void
	{
		makeNewItem()."this"(a)
	}
	Emplace := !(a,b) -> void
	{
		newItm := ref makeNewItem()
		newItm."this"(a,b)
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
	Size := !() -> int
	{
		return itSize
	}
}
