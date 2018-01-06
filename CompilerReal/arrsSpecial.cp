
Node3 := class !{TVal}
{
	Data := TVal
	Prior := int
	Next := Node3.{TVal}^

	this := !(TVal val,int pr) -> void
	{
		Data = val
		Prior = pr
		Next = null
	}
	this := !(TVal val,int pr, Node3.{TVal}^ ne) -> void
	{
		Data = val
		Prior = pr
		Next = ne
	}
}

PriorityStack := class !{TVal}
{
	Start := Node3.{TVal}^

	this := !() -> void
	{
		Start = null
	}
	IsEmpty := !() -> bool
	{
		return Start == null
	}
	Pop := !() -> TVal
	{
		Temp := Start
		Start = Start.Next
		miniTemp := Temp.Data
		free(Temp)
		return miniTemp
	}
	Push := !(TVal val, int pr) -> void
	{		
		if Start == null
		{
			Start = new Node3.{TVal}(val,pr)
		}else{
			iter := Start
			PrevI := Start
			PrevI = null

			lazy := bool
			lazy = iter.Prior < pr

			while lazy
			{
				PrevI = iter
				iter = iter.Next
				lazy = iter != null
				if lazy lazy = iter.Prior < pr
			}

			if PrevI == null
			{
				Start = new Node3.{TVal}(val,pr,Start)
			}else
			{
				PrevI.Next = new Node3.{TVal}(val,pr,PrevI.Next)
			}
		}
	}
	Size := !() -> int
	{
		iter := Start
		ToRet := 0

		while iter != null
		{
			iter = iter.Next
			ToRet += 1
		}
		return ToRet
	}
	GetTopPriority := !() -> int 
	{
		if Start == null return -1
		return Start.Prior	
	}
}

