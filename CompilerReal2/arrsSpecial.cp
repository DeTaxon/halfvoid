
Node3 := class .{@TVal}
{
	Data := TVal
	Next := Node3.{TVal}^
}

PriorityStack := class .{@TVal,@TSize}
{
	Starts := Node3.{TVal}^[TSize]
	Kepts := Node3.{TVal}^
	Counter := int

	this := !() -> void
	{
		for i : TSize Starts[i] = 0
		Kepts = null
		Counter = 0
	}
	Push := !(TVal val, int pr) -> void
	{
		Counter += 1
		newItem := Node3.{TVal}^
		newItem = null
		if Kepts != null
		{
			newItem = Kepts
			Kepts = newItem.Next
		}else{
			newItem = new Node3.{TVal}
		}
		newItem.Data = val
		newItem.Next = Starts[pr]
		Starts[pr] = newItem

	}
	Pop := !() -> TVal
	{
		Counter -= 1

		itItem := Node3.{TVal}^
		for i : TSize
		{
			if Starts[i] != null
			{
				itItem = Starts[i]
				Starts[i] = itItem.Next
				break
			}
		}
		itItem.Next = Kepts
		Kepts = itItem
		return itItem.Data
	}
	IsEmpty := !() -> bool
	{
		return Counter == 0
	}
	GetTopPriority := !() -> int
	{
		for i : TSize
		{
			if Starts[i] != null
			{
				return i
			}
		}
		return -1
	}
}
PriorityQueue := class .{@TVal,@TSize}
{
	Starts := Node3.{TVal}^[TSize]
	Ends := Node3.{TVal}^[TSize]
	Kepts := Node3.{TVal}^
	Counter := int

	this := !() -> void
	{
		for i : TSize Starts[i] = 0
		Kepts = null
		Counter = 0
	}
	Push := !(TVal val, int pr) -> void
	{
		Counter += 1
		newItem := Node3.{TVal}^
		newItem = null
		if Kepts != null
		{
			newItem = Kepts
			Kepts.Next = newItem
		}else{
			newItem = new Node3.{TVal}
		}
		newItem.Data = val
		newItem.Next = null

		if Starts[pr] == null
		{
			Starts[pr] = newItem
			Ends[pr] = newItem
		}else{
			Ends[pr].Next = newItem
			Ends[pr] = newItem
		}
	}
	Pop := !() -> TVal
	{
		Counter -= 1

		itItem := Node3.{TVal}^
		for i : TSize
		{
			if Starts[i] != null
			{
				itItem = Starts[i]
				Starts[i] = itItem.Next
			}
		}
		itItem.Next = Kepts
		Kepts = itItem
		return itItem.Data
	}
	IsEmpty := !() -> bool
	{
		return Counter == 0
	}
	GetTopPriority := !() -> int
	{
		for i : TSize
		{
			if Starts[i] != null
			{
				return i
			}
		}
		return -1
	}
}

