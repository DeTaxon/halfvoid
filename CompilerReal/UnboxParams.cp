
UnboxParams := !(Object^ start) -> void
{
	Bag := Stack.{Object^}

	Curr := start
	while Curr != null
	{
		if Curr.GetValue() == "i:=0"
		{
			iter := Curr.Down
			line := Object^
			line = null

			lineIter := line

			lazy := bool
			lazy = iter != null
	
			StrAdd := string
			while lazy 
			{
				StrAdd = null
				IsStr := false
				if iter.GetValue() == "~ind"
				{
					StrAdd = iter->{ObjIndent^}.MyStr
				}
				if iter.GetValue() == "~str"
				{
					StrAdd = iter->{ObjStr^}.GetString()
					IsStr = true
				}
				
				if StrAdd != null
				{
					if line == null	{
						line = new ObjParam(StrAdd,IsStr)
						lineIter = line
					}else{
						lineIter.Right = new ObjParam(StrAdd,IsStr)
						lineIter = lineIter.Right
					}
				}
				iter = iter.Right
				
				lazy = iter != null
				if lazy  lazy = (iter.GetValue() != ":=")
			}

			IsExt := false
			iter = iter.Right

			if iter.GetValue() == "extern"
			{
				IsExt = true
				iter = iter.Right
			}

			iter.Left = null
			lineIter = line

			while lineIter != null
			{
				((lineIter->{ObjParam^}).IsExtern) = IsExt
				lineIter.Down = iter.Clone()
				lineIter.Down.SetUp(lineIter)
				lineIter = lineIter.Right
			}
			ReplaceNode(Curr,line)

			Curr = line
		}
		Curr = Curr.Right
	}
}
