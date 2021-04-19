UnboxParams := !(Object^ start) -> void
{
	
	Bag := Stack.{Object^}() ; $temp
	Atter := Object^
	Atter = null

	Curr := start
	while Curr != null
	{
		if Curr.GetValue() == "i:=0"
		{
			iter := Curr.Down

			if iter.Right.GetValue() == ":=" and iter.Right.Right.GetValue() == "type"
			{
				itN := iter->{ObjIndent^}
				newObj :=  new TypeDef(itN.MyStr,iter.Right.Right.Right.Clone())
				ReplaceNode(Curr,newObj)
				Curr = newObj

				itrUp := newObj->{Object^}
				if itrUp != null
				{
					while itrUp.Up != null itrUp = itrUp.Up
					asBF := itrUp->{BoxFile^}
					if asBF.cs != null
					{
						asBF.cs.codeTypeDefs[itN.MyStr] = newObj
					}
				}
			}else{

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
					if iter is ObjIndent
					{
						StrAdd = iter->{ObjIndent^}.MyStr
					}else
					if iter is ObjStr
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
				IsRef := false
				IsVir := false
				IsLP := false
				IsTaskLocal := false
				IsWeak := false
				IsPoison := false
				iter = iter.Right

				if iter.GetValue() == "extern"
				{
					IsExt = true
					iter = iter.Right
				}
				if iter.GetValue() == "ref"
				{
					IsRef = true
					iter = iter.Right
				}
				if iter.GetValue() == "virtual"
				{
					IsVir = true
					iter = iter.Right
				}
				if iter.GetValue() == "thread_local"
				{
					IsLP = true
					iter = iter.Right
				}
				if iter.GetValue() == "task_local"
				{
					IsTaskLocal = true
					iter = iter.Right
				}
				if iter.GetValue() == "weak"
				{
					IsWeak = true
					iter = iter.Right
				}
				
				if iter.GetValue() == "poison"
				{
					IsPoison = true
					iter = iter.Right
				}

				if iter.Right != null
				{
					if iter.Right.GetValue() == "at" Atter = iter.Right.Right
				}

				iter.Left = null
				lineIter = line

				while lineIter != null
				{
					asPar := lineIter->{ObjParam^}
					asPar.IsExtern = IsExt
					asPar.IsRef = IsRef
					asPar.IsVirtual = IsVir
					asPar.IsThreadLocal = IsLP
					asPar.IsTaskLocal = IsTaskLocal
					asPar.IsWeak = IsWeak
					asPar.IsPoison = IsPoison
					if IsVir WorkBag.Push(asPar,State_Start)
					lineIter.Down = iter.Clone()
					lineIter.Down.SetUp(lineIter)
					if Atter != null {
						((lineIter->{ObjParam^}).Atter) = Atter.Clone()
					}
					lineIter = lineIter.Right
				}
				ReplaceNode(Curr,line)

				Curr = line
			}
		}
		Curr = Curr.Right
	}
}
