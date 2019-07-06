
MiniMachineNode := class 
{
	WhatNext := Map.{string,MiniMachineNode^}
	IsTerm := bool

	this := !() -> void
	{
		WhatNext.Start = null
		IsTerm = false
	}
	GetNext := !(string item) -> MiniMachineNode^
	{
		if not WhatNext.Exist(item) return null
		return WhatNext[item]
	}
}

PriorityBag := class
{
	Opers := QueueSet.{string}
	Lines := Queue.{MiniMachineNode^}

	this := !(char^ ptrChar, int mapSize) -> void
	{
		Opers.Start = null
		Lines.Start = null
		

		itS := 0
		while itS < mapSize
		{
			CurWords := Queue.{StringSpan}() ; $temp

			j := itS
			isLoading := false
			while itS < mapSize and ptrChar[itS] != '\n'
			{
				if isLoading
				{
					if ptrChar[itS] == '\"'
					{
						CurWords.Emplace(ptrChar[j]&,itS - j)
						isLoading = false
					}
				}else
				{
					if ptrChar[itS]  == '\"'
					{
						isLoading = true
						j = itS + 1
					}
				}
				itS++
			}
			itS++

			if CurWords.NotEmpty()
			{
				newMach := new MiniMachineNode()

				for rule : CurWords
				{
					items := rule.DivideStr(" ")

					machIter := newMach

					//while someWord != null
					for word : items
					{
						stW := word.Str()
						switch stW
						{
							case "d"
								stW = "~d"
							case "s"
								stW = "~suffix"
							case "()"
							case "[]"
							case "{}"
							case void
								Opers.Push(stW)
						}

						k := ref machIter.WhatNext[stW]

						if k == null
							k =  new MiniMachineNode()
						machIter = k
					}
					machIter.IsTerm = true
				}
				Lines.Push(newMach)
			}
		}
	}
}

