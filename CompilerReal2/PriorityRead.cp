
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

	this := !(string filename) -> void
	{
		Opers.Start = null
		Lines.Start = null
		itFile := sfile(filename,"r")
		
		buffer := char[1024]

		while itFile.good()
		{
			CurLine := itFile.readline()
			CurWords := Queue.{string}() ; $temp

			i := 0
			j := 0
			
			isLoading := false
			while CurLine[i] != 0
			{
				if isLoading
				{
					if CurLine[i] == '\"'
					{
						buffer[j] = 0
						CurWords.Push(buffer.Copy())			
						j = 0
					}else
					{
						buffer[j] = CurLine[i]
						j += 1
					}
				}else
				{
					if CurLine[i]  == '\"'
					{
						isLoading = true
					}
				}
				i += 1
			}

			if CurWords.NotEmpty()
			{
				newMach := new MiniMachineNode()
				items := Queue.{string}() ; $temp

				ruleIter := CurWords.Start

				while ruleIter != null
				{
					items.Clean()
					DivideStr(ruleIter.Data,' ',items)

					someWord := items.Start
					machIter := newMach

					while someWord != null
					{
						if someWord.Data == "d" someWord.Data = "~d" else 
						if someWord.Data == "s" someWord.Data = "~suffix" else
						{
							if someWord.Data != "()" and someWord.Data != "[]" and someWord.Data != "{}"
								Opers.Push(someWord.Data)
						}

						if not machIter.WhatNext.Exist(someWord.Data)
						{
							machIter.WhatNext[someWord.Data] =  new MiniMachineNode()
						}
						machIter = machIter.WhatNext[someWord.Data]
						someWord = someWord.Next
						if someWord == null machIter.IsTerm = true
					}
					ruleIter = ruleIter.Next
				}
				Lines.Push(newMach)
			}
		}
		itFile.close()
	}
}

