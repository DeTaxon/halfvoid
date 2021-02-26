
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

MiniMachineNode2 := class 
{
	WhatNext := MiniMachineNode2^[256]
	IsTerm := bool

	this := !() -> void
	{
		WhatNext."this"()
		IsTerm = false
	}

	FromMach1 := !(PriorityBag^ base,MiniMachineNode^ nd) -> void
	{
		IsTerm = nd.IsTerm
		for it,ind : nd.WhatNext
		{
			if it == null continue
			itId := base.StrToId[ind]
			assert(itId != 0)
			newNode := new MiniMachineNode2
			WhatNext[itId] = newNode
			newNode.FromMach1(base,it)
		}
	}
}

PriorityBag := class
{
	Opers := AVLSet.{string}
	Lines := Queue.{MiniMachineNode^}
	LinesRaw := Queue.{char^}

	TotalOpers := AVLSet.{char^}
	StrToId := AVLMap.{char^,int}

	Lines2 := List.{MiniMachineNode2^}

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
		DoStep2()
	}
	DoStep2 := !() -> void
	{
		TotalOpers.Insert(Opers[^])
		TotalOpers.Insert(LexKeyWords[^])
		TotalOpers.Insert(AllKeyWords[^])

		for it,i : TotalOpers
		{
			StrToId[it] = i + 50
		}
		StrToId["~d"] = 39
		StrToId["()"] = 46
		StrToId["{}"] = 47
		StrToId["[]"] = 48
		StrToId["~suffix"] = 49
		
		for it : Lines
		{
			if it == null {
				Lines2.Push(null)
				continue
			}
			newM := new MiniMachineNode2
			newM.FromMach1(this&,it)
			Lines2.Push(newM)
		}
	}
}

LexKeyWords := ![":=","=>","==>","...",":",".",",","?","!",".","|"]
AllKeyWords := !["return","for","if","else","while","do","delete","switch","case" 
,"class","extern" ,"extend" ,"at" 
,"defer" ,"type" ,"virtual" ,"new" 
,"delete" ,"thread_local" ,"task_local" ,"keep_name" 
,"packed_class" ,"self_return" ,"try" ,"catch" 
,"throw" ,"on_exception" ,"yield" ,"AppendClass" ,"default" ]

