
FuncCallSpecific := !(Object^ iter) -> Object^
{
	if iter.Down?.Right?.GetValue() == "->"
	{
		if iter.Down is ObjType
		{
			asTyp := iter.Down->{ObjType^}.MyType
			if iter.Down.Right.Right? is ObjIndent
			{
				asObj := iter.Down.Right.Right->{ObjIndent^}
				if asObj.MyStr == "Len"
				{
					if asTyp is TypeArr
					{
						return new ObjInt(asTyp->{TypeArr^}.Size)
					}
				}
				if asObj.MyStr == "Base"
				{
					if asTyp is TypeArr or asTyp is TypePoint it asTyp is TypeFatArr
					{
						return new ObjType(asTyp.Base)
					}
				}
			}
		}
		if iter.Down is ParamNaturalCall
		{
			if iter.Down.Right.Right? is ObjIndent
			{
				asObj := iter.Down.Right.Right->{ObjIndent^}
				if asObj.MyStr == "ptr"
				{
					iter.Down->{ParamNaturalCall^}.isUnref = true
					iter.Down.Right = null
					return iter.Down
				}
			}
		}
		if iter.Down.Right.Right? is ObjIndent
		{	
			asObj := iter.Down.Right.Right->{ObjIndent^}
			if asObj.MyStr == "SetType" and iter.Down.Right.Right.Right != null
			{
				iter.Print(0)
			}
		}
	}
	if iter.Down?.Right?.GetValue() == "is"
	{
		asCl1 := iter.Down.GetType()
		if not(asCl1 == null or asCl1 is TypePoint or asCl1.Base is TypeClass) return null
		asCl2 := ParseType(iter.Down.Right.Right)
		if asCl2 == null or not asCl2 is TypeClass return null

		asNeed := asCl1.Base->{TypeClass^}.ToClass
		
		itBox := new FuncInputBox()  ; $temp

		itBox.itPars.Emplace(asCl1.Base,true)
		itBox.itConsts.Push(new ObjType(asCl2)) 
		
		func := asNeed.VirtualCheck.GetFunc(itBox^)
		if func == null return null
		PopOutNode(iter.Down.Right)
		PopOutNode(iter.Down.Right)
		return MakeSimpleCall(func,iter.Down)

	}
	return null
}
