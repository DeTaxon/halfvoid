Node := class .{@T}
{
	Data := T
	Next := Node.{T}^
	this := !() -> void
	{
	}
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
	itSize := int
	this := !() -> void
	{
		Start = null
		itSize = 0
	}
	Front := !() -> ref T { return Start.Data }
	Emplace := !() -> void
	{
		newItm := new Node.{T}() //TODO: vargs
		newItm.Next = Start
		Start = newItm
		itSize += 1
	}
	Push := !(T a) -> int
	{
		Start =	new Node.{T}(a,Start)
		itSize += 1
		return 0
	}
	Pop := !() -> T
	{
		Prev := Start.Data
		Later := Start
		Start = Start.Next
		//delete Later
		itSize--
		return Prev
	}
	Size := !() -> int
	{
		return itSize
		//Count := 0
		//Iter := Start
		//while Iter 
		//{
		//	Count += 1
		//	Iter = Iter.Next
		//}
		//return Count
	}
	Clean := !() -> void
	{
		Iter := Node.{T}^
		while Start
		{
			Iter = Start
			Start = Iter.Next
			delete Iter
		}
		itSize = 0

	}
	"~this" := !() -> void
	{
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
	ToArray := !() -> T^
	{
		Si := itSize
		//if Si == 0 return null->{T[]}
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
		for toSet Push(it)
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
		itSize = 0
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
		itSize++
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
		itSize++
		return 0
	}
	"=" := !(Stack.{T} toSet) -> void
	{
		for toSet Push(it)
	}
}
QueueSet := class .{@T} extend Stack.{T}
{
	this := !() -> void
	{
		Start = null
		itSize = 0
	}
	Push := !(T a) -> int
	{
		if Start == null {
			Start = new Node.{T}(a)
			itSize++
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
			itSize++
		}
		return 0
	}
	GetPos := !(T  itm) -> int
	{
		i := 0
		for this
		{
			if it == itm return i
			i++
		}
		return -1
	}
	Contain := !(T itm) -> bool
	{
		for this 
			if it == itm return true
		return false
	}
	"<<" := !(T itm) -> ref QueueSet.{T}
	{
		Push(itm)
		return this
	}
	"<<<" := !(T itm) -> int
	{
		for it : this , i : 0
			if it == itm return i
		if Start == null
		{
			Start = new Node.{T}()
			Start.Data <<< itm
			itSize++
			return 0
		}
		itr := Start
		while itr.Next != null	itr = itr.Next
		itr.Next = new Node.{T}()
		itr = itr.Next
		itr.Data <<< itm
		return itSize++
	}
	"<<<" := !(QueueSet.{T} toAdd) -> void
	{
		itSize = toAdd.itSize
		Start = toAdd.Start
		toAdd.itSize = 0
		toAdd.Start = null
	}
	"==" := !(QueueSet.{T} toCmp) -> bool
	{
		if toCmp.Size() != itSize return false

		for this
			if not toCmp.Contain(it) return false

		return true
	}
	"=" := !(Stack.{T} toSet) -> void
	{
		for toSet Push(it)
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
	Empty := !() -> bool
	{
		return Start == null
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

