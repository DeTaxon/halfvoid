
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
	Prior := int
	Depth := int

	this := !() -> void
	{
		WhatNext."this"()
		Prior = 0
	}

	FromMach1 := !(PriorityBag^ base,int priVal,MiniMachineNode^ nd) -> void
	{
		if nd.IsTerm Prior = priVal
		for it,ind : nd.WhatNext
		{
			if it == null continue
			itId := base.StrToId[ind]
			assert(itId != 0)
			if WhatNext[itId] == null WhatNext[itId] = new MiniMachineNode2
			WhatNext[itId].FromMach1(base,priVal,it)
		}
	}
	GetDepth := !() -> int
	{
		maxVal := 0
		for it : WhatNext
		{
			if it == null continue
			itVal := it.GetDepth()
			if itVal > maxVal itVal = maxVal
		}
		return maxVal + 1
	}
	CheckDepth := !() -> void
	{
		Depth = GetDepth()
	}
}

PriorityBag := class
{
	Opers := AVLSet.{string}
	Lines := Queue.{MiniMachineNode^}
	LinesRaw := Queue.{char^}

	TotalOpers := AVLSet.{char^}
	StrToId := AVLMap.{char^,int}

	Lines2 := List.{Tuple.{MiniMachineNode2^,char^}}

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
	
	AddExtra := !(AVLSet.{char^} toAdd) -> void
	{
		LexKeyWords := ![":=","=>","==>","...",":",".",",","?","!",".","|"]
		AllKeyWords := !["return","for","if","else","while","do","delete","switch","case" 
		,"class","extern" ,"extend" ,"at" 
		,"defer" ,"type" ,"virtual" ,"new" 
		,"delete" ,"thread_local" ,"task_local" ,"keep_name" 
		,"packed_class" ,"self_return" ,"try" ,"catch" 
		,"throw" ,"on_exception" ,"yield" ,"AppendClass" ,"default" ,"weak"]

		toAdd.Insert(LexKeyWords[^])
		toAdd.Insert(AllKeyWords[^])
	}
	DoStep2 := !() -> void
	{

		TotalOpers.Insert(Opers[^])
		AddExtra(TotalOpers)

		for it,i : TotalOpers
		{
			StrToId[it] = i + 50
		}
		StrToId["~d"] = 39
		StrToId["()"] = 46
		StrToId["{}"] = 47
		StrToId["[]"] = 48
		StrToId["~suffix"] = 49
	
		cpMach := MiniMachineNode2^()
		for it,i : Lines, raw : LinesRaw
		{
			if it == null {
				if cpMach != null
				{
					Lines2.Emplace(cpMach,null)
					cpMach = null
				}
				Lines2.Emplace(null,raw)
				continue
			}
			if cpMach == null
				cpMach = new MiniMachineNode2
			cpMach.FromMach1(this&,i + 1,it)

			Lines2.Emplace(cpMach,null)
			cpMach = null
		}
		if cpMach != null
			Lines2.Emplace(cpMach,null)
		for it : Lines2
		{
			if it.0 != null
				it.0.CheckDepth()
		}
	}
}


