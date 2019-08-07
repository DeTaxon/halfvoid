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

	this := !() -> void
	{
		Start = null
		End = null
		Counter = 0
	}
	Push := !(T toAdd) .{} -> void
	{
		this << toAdd
	}
	Emplace := !(args...) -> void
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

	"<<" := !(T toAdd) .{} -> ref List.{T}
	{

		if $uniq 
		{
			if this[^] == toAdd
				return this
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
	Pop := !()  .{} -> T
	{
		oldVal := Start.Data
		oldNode := Start
		Start = Start.Next
		if $keep 
		{
			oldNode = CreatedNodes
			CreatedNodes = oldNode
		}else{
			if not $temp
				delete oldNode
		}
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
