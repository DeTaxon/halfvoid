

FuncCallTryCompute := !(Object^ iter) -> Object^
{
	if iter.Down?.IsConst and IsOper(iter.Down.Right?.GetValue()) and iter.Down.Right.Right?.IsConst and iter.Down.Right.Right.Right == null
	{
		oprStr := iter.Down.Right.GetValue()
		switch  oprStr
		{
		case "=="
			if iter.Down is ObjStr  and iter.Down.Right.Right is ObjStr
			{
				return new ObjBool(iter.Down->{ObjStr^}.GetString() == iter.Down.Right.Right->{ObjStr^}.GetString())
			}
		case "and"
			if iter.Down is ObjBool and iter.Down.Right.Right.GetType() == GTypeBool
			{
				//inA := iter.Down->{ObjBool^}.MyBool
				//if not inA
				//	return new ObjBool(false)
				//return iter.Right.Right
			}
		case "or"
		}

	}
	return null
}
