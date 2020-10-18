ListNode := class .{@T}
{
	Data := T
	Next := ListNode.{T}^
	this := !() -> void
	{
	}
	this := !(T To) -> void
	{
		Data = To
		Next = null
	}
	this := !(T To,ListNode.{T}^ Ne) -> void
	{
		Data = To
		Next = Ne
	}
}

ListIter := class .{@T}
{
	iter := ListNode.{T}^
	this := !(ListNode.{T}^ toStart) -> void {	iter = toStart }
	"^" := !() -> ref T { return iter.Data }
	Inc := !() -> void { iter = iter.Next }
	IsEnd := !() -> bool { return iter == null }
}

List := class .{@T}
{
	Start := ListNode.{T}^
	End := ListNode.{T}^
	Counter := int

	if $keep
		CreatedNodes := ListNode.{T}^

	this := !() .{} -> void
	{
		Start = null
		End = null
		if $keep
			CreatedNodes = null
		Counter = 0
	}
	Push := !(T toAdd) .{} -> void
	{
		this << toAdd
	}
	EmplaceFront := !(args...) -> ref T
	{
		if $uniq 
		{
			if this[^] == toAdd
				return it
		}

		newNode := ListNode.{T}^()

		if $keep 
		{
			if CreatedNodes != null
			{
				newNode = CreatedNodes
				CreatedNodes = CreatedNodes.Next
			}
		}
		if newNode == null
		{
			newNode = new ListNode.{T}
		}
		newNode.Data."this"(args...)
		newNode.Next = Start
		Start = newNode
		if End == null End = Start
		Counter++
		return newNode.Data
	}
	Emplace := !(args...) -> ref T
	{
		if $uniq 
		{
			if this[^] == toAdd
				return it
		}

		newNode := ListNode.{T}^()

		if $keep 
		{
			if CreatedNodes != null
			{
				newNode = CreatedNodes
				CreatedNodes = CreatedNodes.Next
			}
		}
		if newNode == null
		{
			newNode = new ListNode.{T}
		}
		newNode.Data."this"(args...)
		newNode.Next = null
		if End == null
		{
			End = newNode
			Start = newNode
		}else{
			End.Next = newNode
			End = newNode
		}

		Counter++

		return End.Data
	}
	DeleteIf := !(!(T)&->bool testFunc) -> void //TODO: why cant i make it template
	{
		if Counter == 0 return void

		prevNode := ListNode.{T}^()
		itrNode := Start
		while itrNode != null
		{
			if testFunc(itrNode.Data)
			{
				toDel := itrNode
				Counter -= 1
				if prevNode == null
				{
					Start = itrNode.Next
					if End == toDel End = Start
					itrNode = Start
				}else{
					prevNode.Next = itrNode.Next
					if End == toDel End = prevNode
					itrNode = prevNode.Next
				}
				destroyNode(toDel)
			}else{
				prevNode = itrNode
				itrNode = itrNode.Next
			}
		}
	}
	DeleteAt := !(int pos) -> void
	{
		oldNode := ListNode.{T}^
		if pos == 0
		{
			oldNode = Start
			if Counter == 1
			{
				Start = null
				End = null
			}else{
				Start = Start.Next
			}
			
		}else{
			itr := Start
			for (pos-1) {
				itr = itr.Next
			}
			oldNode = itr.Next
			if End == oldNode
			{
				End = itr
			}
			itr.Next = itr.Next.Next
		}
		if $keep
		{
			oldNode.Next = CreatedNodes
			CreatedNodes = oldNode
		}else{
			if not $temp
				delete oldNode
		}
		Counter--
	}
	Front := !() -> ref T
	{
		return Start.Data
	}
	Back := !() -> ref T
	{
		return End.Data
	}

	"[]" := !(int x) -> ref T
	{
		iitt := Start
		for x
			iitt = iitt.Next
		return iitt.Data
	}

	createNode := !() .{} -> ListNode.{T}^
	{
		newNode := ListNode.{T}^()

		if $keep
		{
			if CreatedNodes != null
			{
				newNode = CreatedNodes
				CreatedNodes = CreatedNodes.Next
			}
		}
		if newNode == null
		{
			newNode = new ListNode.{T}
		}
		return newNode
	}
	"<<" := !(T toAdd) .{} self_return
	{

		if $uniq 
		{
			if this[^] == toAdd
				return this
		}

		newNode := this.createNode()

		newNode.Data = toAdd
		newNode.Next = null
		if End == null
		{
			End = newNode
			Start = newNode
		}else{
			End.Next = newNode
			End = newNode
		}

		Counter++

		return this
	}
	CreateBeforeIf := !(!(T&)&->bool cmpTst)  -> ref T
	{

		prevNode := ListNode.{T}^()
		listIter := Start
		while listIter != null
		{
			if not cmpTst(listIter.Data)
				break
			prevNode = listIter
			listIter = listIter.Next
		}
		newNode := this.createNode()
		Counter++
		if prevNode == null
		{
			newNode.Next = Start
			Start = newNode
			if End == null End = Start
			return newNode.Data
		}
		newNode.Next = prevNode.Next->{void^}
		prevNode.Next = newNode
		return newNode.Data
	}
	InsertBeforeIf := !(T newValue,!(T&)&-> bool cmpTst) .{} -> void
	{
		this.CreateBeforeIf(cmpTst) = newValue
	}
	"in" := !(T toCmp) .{} -> bool
	{
		if this[^] == toCmp
			return true
		return false
	}
	PushFront := !(T toAdd) .{} -> void
	{

		if $uniq 
		{
			if this[^] == toAdd
				return void
		}

		newNode := ListNode.{T}^()

		if $keep
		{
			if CreatedNodes != null
			{
				newNode = CreatedNodes
				CreatedNodes = CreatedNodes.Next
			}
		}
		if newNode == null
		{
			newNode = new ListNode.{T}
		}

		newNode.Data = toAdd
		newNode.Next = Start
		Start = newNode
		Counter++

		return void
	}
	destroyNode := !(ListNode.{T}^ toDest) .{} -> void
	{
		if $temp return void

		if $keep 
		{
			toDest.Next = CreatedNodes
			CreatedNodes = toDest
		}else{
			delete toDest
		}
	}
	Pop := !()  .{} -> T
	{
		oldVal := Start.Data
		oldNode := Start
		Start = Start.Next
		this.destroyNode(oldNode)

		if Start == null End = null

		Counter--
		return oldVal
	}
	IsEmpty := !() -> bool
	{
		return Counter == 0
	}
	"~For" := !() -> ListIter.{T}
	{
		return ListIter.{T}(Start)
	}
	Size := !() -> int
	{
		return Counter
	}
	ToArray := !() .{} -> T[]
	{
		if Counter == 0 
			return null->{T[]}

		preRes := new T[Counter]
		preRes[i] = this[^i]
		return preRes
	}
	Clear := !() .{} -> void
	{
		if $keep
		{
			if Counter != 0
			{
				End.Next = CreatedNodes
				CreatedNodes = Start
			}
		}else{
			Destroy()
		}
		Start = null
		End = null
		Counter = 0
	}
	Destroy := !() .{} -> void
	{
		if not $temp
		{
			if $keep 
			{
				while CreatedNodes != null
				{
					itNd := CreatedNodes
					CreatedNodes = CreatedNodes.Next
					delete itNd
				}
			}
			if Counter != 0
			{
				while Start != null
				{
					itNd := Start
					Start = Start.Next
					delete itNd
				}
			}
		}
		Start = null
		End = null
		Counter = 0
		
	}
	"<<<" := !(List.{T} toAdd) -> ref List.{T}
	{
		if toAdd.Counter == 0
			return this
		if Counter == 0
		{
			Start = toAdd.Start
			End = toAdd.End
			Counter = toAdd.Counter
			toAdd.Start = null
			toAdd.End = null
			toAdd.Counter = 0
			return this
		}
		End.Next = toAdd.Start
		End = toAdd.End
		toAdd.Start = null
		toAdd.End = null
		toAdd.Counter = 0
		Counter += toAdd.Counter
	}
}
