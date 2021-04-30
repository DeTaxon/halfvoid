


FuncCallCheckCreateArray := !(Object^ ToParse) -> Object^
{
    iter := ToParse.Down
    if iter?.GetValue() == "!"
	{
		if iter.Right?.GetValue() == "[]"
		{
			tryCmp := TryCompute(ToParse)
			if tryCmp != null{
				return tryCmp.Clone()
			}
			box := new FuncInputBox()  ; $temp

			for iterY : iter.Right.Down
			{
				if iterY.GetValue() != ","
				{
					box.itPars.Emplace(iterY.GetType(),iterY.IsRef())
				}
			}
			f := FindFunc("![]",iter,box^,false)

			if f != null
			{
				TrimCommas(iter.Right)
				return MakeSimpleCall2(f,iter.Right.Down,iter)
			}
		}
		if iter.Right.GetValue() == "{}" or iter.Right.GetValue() == "{d}"
		{
			box := new FuncInputBox() ; $temp

			if iter.Right.Down[^].GetValue() != ","
			{
				if it == null or it.GetType() == null
					return null
				box.itPars.Emplace(it.GetType(),it.IsRef())
			}

			CTT2 := CTT->{BoxTemplate^}

			f := CTT2.GetFunc(box^)
			if f != null
			{
				TrimCommas(iter.Right)
				return MakeSimpleCall(f,iter.Right.Down)
			}
		}
		return null
	}
    return null
}

FuncCallGetArrayElement := !(Object^ iterPre,Object^ iter) -> Object^
{
    if iter.GetValue() == "[]"
    {
        iter = iter.Left
        iter.Right = iter.Right.Down
        iter.Right.Left = iter
        iter.SetUp(iter.Up)
        return OneCall("[]",iter.Up,null->{Object^})
    }
    return null
}