#import "lib.cp"

Node := class .{@T}
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


StackTypeIter := class .{@T}
{
	iter := Node.{T}^
	this := !(Node.{T}^ toStart) -> void {	iter = toStart }
	"^" := !() -> ref T { return iter.Data }
	Inc := !() -> void { iter = iter.Next }
	IsEnd := !() -> bool { return iter == null }
}


Stack := class .{@T}
{
	Start := Node.{T}^
	this := !() -> void
	{
		Start = null
	}
	Push := !(T a) -> int
	{
		if Start == null {
			Start = new Node.{T}(a)
		} else {
			Start = new Node.{T}(a,Start)
		}
		return 0
	}
	Pop := !() -> T
	{
		Prev := Start.Data
		Later := Start
		Start = Start.Next
		free(Later)
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
	"~this" := !() -> void
	{
		printf("teest\n")
		Clean()
	}
	NotEmpty := !() -> bool
	{
		return Start != null
	}
	Empty := !() -> bool
	{
		return Start == null
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
	ToArray := !() -> T[]
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
	"=" := !(Stack.{T} toSet) -> void
	{
		Start = toSet.Start
	}
	"~For" := !() -> StackTypeIter.{T}
	{
		return StackTypeIter.{T}(Start)
	}
}

Queue := class .{@T} extend Stack.{T}
{
	this := !() -> void
	{
		Start = null
	}
	"=" := !(Queue.{T} toSet) -> void
	{
		for toSet.Size() Push(toSet[it])
	}
	PushFront := !(T a) -> int
	{
		if Start == null{
			Start = new Node.{T}(a)
		}else{
			Start = new Node.{T}(a,Start)
		}
		return 0
	}
	Push := !(T a) -> int
	{
		if Start == null {
			Start = new Node.{T}(a)
		} else {
			Iter := Start
			while Iter.Next {Iter = Iter.Next}
			Iter.Next = new Node.{T}(a)
		}
		return 0
	}
}
QueueSet := class .{@T} extend Stack.{T}
{
	this := !() -> void
	{
		Start = null
	}
	Push := !(T a) -> int
	{
		if Start == null {
			Start = new Node.{T}(a)
		} else {
			Iter := Start
			while Iter.Next 
			{
				if Iter.Data == a
					return 0

				Iter = Iter.Next
			}
			if Iter.Data == a
				return 0
			Iter.Next = new Node.{T}(a)
		}
		return 0
	}
}

DoubleNode := class .{@TKey,@TValue}
{
	Key := TKey
	Value := TValue
	Next := DoubleNode.{TKey,TValue}^

	this := !(TKey k, DoubleNode.{TKey,TValue}^ v) -> void
	{
		Key = k
		Next = v
	}
	this := !(TKey k,TValue val, DoubleNode.{TKey,TValue}^ v) -> void
	{
		Key = k
		Value = val
		Next = v
	}
}

Map := class .{@TKey,@TValue}
{
	Start := DoubleNode.{TKey,TValue}^
	this := !() {
		Start = null
	}
	"[]" := !(TKey look) -> ref TValue
	{
		iter := Start
		while iter
		{
			if iter.Key == look 
			{
				return iter.Value
			}
			iter = iter.Next
		}
		Start = new DoubleNode.{TKey,TValue}(look,Start)
		return Start.Value
	}
	"=" := !(Map.{TKey,TValue} toSet) ->void
	{
		Start = toSet.Start
	}
	Set := !(TKey k, TValue v) -> void
	{
		iter := Start
		while iter
		{
			if iter.Key == k 
			{
				iter.Value = v
			}
			iter = iter.Next
		}
		Start = new DoubleNode.{TKey,TValue}(k,Start)
		Start.Value = v
	}
	Exist := !(TKey key) -> bool
	{
		iter := Start
		while iter != null
		{
			if iter.Key == key return true
			iter = iter.Next
		}
		return false
	}
	Delete := !() -> void
	{
		iter := Start
		while iter 
		{
			Temp := iter.Next
			free(iter)
			iter = Temp
		}
	}
	
}

