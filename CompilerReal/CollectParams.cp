
CollectParams := !(Object^ start) -> void
{
	Bag := Stack.{Object^}

	Bag.Push(start)

	while Bag.NotEmpty()
	{
		Curr := Bag.Pop()

		if Curr.GetValue() == "i:=0"
		{
			//printf("item %s\n",Curr.Down.Right.Right.GetValue())
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
				lineIter = lineIter.Right
			}
			ReplaceNode(Curr,line)
			Bag.Push(line)

		}else{
		      if Curr.Down  != null Bag.Push(Curr.Down)
		      if Curr.Right != null Bag.Push(Curr.Right)
		}
	}
}
