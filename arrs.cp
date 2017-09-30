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
	this := !() -> void
	{
		Start = null
	}
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
	NotEmpty := !() -> bool
	{
		return Start != null
	}
	"[]" := !(int Ind) -> ref T
	{
		Iter := Start
		j := Ind
		while j
		{
			Iter = Iter.Next
			j -= 1
		}
		return Iter.Data		
	}
	ToArray := !() -> T^
	{
		Si := this.Size()
		if Si == 0 return null
		ToOut := new T[Si]
		Iter := Start
		i := 0
		while Iter
		{
			ToOut[i] = Iter.Data
			Iter = Iter.Next
			i += 1	
		}
		return ToOut
	}
	"for" := !() -> ref T
	{
		Iter := Start
		while Iter
		{	
			block(Iter.Data)
			Iter = Iter.Next
		}
	}
}

Queue := class !{T} extend Stack.{T}
{
	this := !() -> void
	{
		Start = null
	}
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
