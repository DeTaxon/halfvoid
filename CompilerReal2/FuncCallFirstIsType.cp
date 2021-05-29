FuncCallFirstIsType := !(Object^ ToParse) -> Object^
{
    iter := ToParse.Down
    if iter == null return null

    fastType := Type^
	fastType = null
	
	if iter.Right?.GetValue() == "()"
	{	
		fastType = ParseType(iter)
	}
	if iter is ObjType //iter.GetValue() == "~type"
	{
		asNeed := iter->{ObjType^}
		fastType = asNeed.MyType
	}

	if fastType != null
	{
		//asNeed := iter->{ObjType^}
		asNeed2 := fastType

		if asNeed2 is TypeClass and iter.Right.GetValue() == "()"
		{
			asNeed3 := asNeed2->{TypeClass^}
			asNeed4 := asNeed3.ToClass
			
			box := new FuncInputBox()  ; $temp
			FillAttrs(box^,iter)
			box.itPars.Emplace(asNeed2,true) 
			TrimCommas(iter.Right)

			for iter2 : iter.Right.Down
			{
				box.itPars.Emplace(iter2.GetType(),iter2.IsRef())
			}

			func := asNeed4.GetFunc("this",box^,true)
			if func != null 
			{
				preRet := new ConstructCall(func,iter.Right.Down)
				preRet.Line = iter.Line
				return preRet
			}
		}


		if iter.Right.GetValue() == "->"
		{
			if iter.Right.Right is ObjIndent //iter.Right.Right.GetValue() == "~ind"
			{
				tt := iter.Right.Right
				asNeed := tt->{ObjIndent^}

				if iter is ObjType
				{
					box := new FuncInputBox()  ; $temp

					objT := iter->{ObjType^}

					box.itConsts.Push(objT)
					box.itConsts.Push(new ObjStr(asNeed.MyStr)) 
					func := FindFunc("->",iter,box^,false)
					if func != null {
						return MakeSimpleCall(func,null->{Object^})
					}
				}

				box := new FuncInputBox()  ; $temp


				box.itConsts.Push(iter)
				box.itConsts.Push(new ObjStr(asNeed.MyStr)) 
				
				func := FindFunc("->", iter,box^,false)
				if func != null return MakeSimpleCall(func,null->{Object^})
			}
		}

		if asNeed2 is TypeStandart or asNeed2 is TypePoint or asNeed2 is TypeFatArr
		{
			if iter.Right?.GetValue() == "()"
			{
				Cs := Queue.{Object^}()
				Cs.Push(new ObjType(asNeed2)) ; $temp 
				return OneCall(". this",iter.Right,Cs,false)
			}
		}
		return null
	}
    return null
}