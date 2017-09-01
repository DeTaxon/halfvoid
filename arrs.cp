Node := class !{T}
{
	Data := T
	Next := Node.{T}^
	this := !(T To) -> void
	{
		Data = To
		Next = null
	}
	this := !(T To,Node.{T}^ Ne) -> void
	{
		Data = To
		Next = Ne
	}
}

Stack := class !{T}
{
	Start := Node.{T}^
	Push := !(T a) -> void
	{
		if Start == null {
			Start = new Node.{T}(a)
		} else {
			Start = new Node.{T}(a,Start)
		}
	}
	Pop := !() -> T
	{
		Prev := Start.Data
		Later := Start.Next
		free(Start)
		Start = Later
		return Prev
	}
	Size := !() -> int
	{
		Count := 0
		Iter := Start
		while Iter 
		{
			Count += 1
			Iter = Iter.Next
		}
		return Count
	}
	Clean := !() -> void
	{
		Iter := Node.{T}^
		while Start
		{
			Iter = Start
			Start = Iter.Next
			free(Iter)
		}	
	}
}

Queue := class !{T} extend Stack.{T}
{
	Push := !(T a) -> void
	{
		if Start == null {
			Start = new Node.{T}(a)
		} else {
			Iter := Start
			while Iter.Next Iter = Iter.Next
			Iter.Next = new Node.{T}(a)
		}
	}
}
