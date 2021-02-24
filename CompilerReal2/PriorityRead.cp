
MiniMachineNode := class 
{
	WhatNext := AVLMap.{char^,MiniMachineNode^}
	IsTerm := bool

	this := !() -> void
	{
		WhatNext."this"()
		IsTerm = false
	}
	GetNext := !(string item) -> MiniMachineNode^
	{
		inIt := WhatNext.TryFind(item)
		if inIt != null return inIt^
		return null
	}
}

PriorityBag := class
{
	Opers := AVLSet.{string}
	Lines := Queue.{MiniMachineNode^}
	LinesRaw := Queue.{char^}

	this := !(char^ ptrChar, int mapSize) -> void
	{
		Opers."this"()
		Lines."this"()
		LinesRaw."this"()
		

		itS := 0
		while itS < mapSize
		{
			CurWords := Queue.{StringSpan}() ; $temp

			if ptrChar[itS] == '#'
			{
				strtT := itS
				while itS < mapSize and ptrChar[itS] != '\n'
				{
					itS++
				}
				nowLineQ := ptrChar[strtT..(itS - strtT)]
				nowLine := nowLineQ.Str()
				LinesRaw.Push(nowLine)
				Lines.Push(MiniMachineNode^())
				itS++
				continue
			}else{
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
			}
			LinesRaw.Push(char^())

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
								Opers.Insert(stW)
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

DoKeys := !(!(char^)& -> void kvs ) -> void
{
	for it : ![":=","=>","==>","...",":"]
	{
		kvs(it)	
	}
}


