
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
				if iter.GetValue() == "~ind"
				{
					StrAdd = iter->{ObjIndent^}.MyStr
				}
				
				if StrAdd != null
				{
					if line == null	{
						line = new ObjParam(StrAdd)
						lineIter = line
					}else{
						lineIter.Right = new ObjParam(StrAdd)
						lineIter = lineIter.Right
					}
				}
				iter = iter.Right
				
				lazy = iter != null
				if lazy  lazy = (iter.GetValue() != ":=")
			}

			iter = iter.Right
			lineIter = line

			while lineIter != null
			{
				lineIter.Down = iter
				iter.SetUp(lineIter)
				lineIter = lineIter.Right
			}
			ReplaceNode(Curr,line)
			Curr = line
		}
		Curr = Curr.Right
	}
}
