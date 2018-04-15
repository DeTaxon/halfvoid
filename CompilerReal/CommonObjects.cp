
GetItem := !(string name, Object^ start) -> Object^
{
	iter := start

	while iter != null
	{
		if iter.GetValue() == "i:=1"
		{
			AsNeed := iter->{ObjParam^}
			if (AsNeed.MyStr == name) {
				if not AsNeed.AskedGetUse and AsNeed.IsFunc WorkBag.Push(iter,State_GetUse)
				return iter
			}
		}
		if iter.GetValue() == ":=type"
		{
			asDef := iter->{TypeDef^}
			if asDef.ItName == name return iter
		}
		if iter.GetValue() == "(const)"
		{
			asC := iter->{ConstItem^}
			if asC.Name == name return iter
		}
		if iter.GetValue() == "~for()"
		{
			asC := iter->{BoxFor^}
			if asC.itName == name return asC.LocPar
			if asC.indName == name return asC.IndPar
		}

		if iter.Left != null iter = iter.Left 
		else
		{
			iter = iter.Up

			if iter != null
			{
				if iter.GetValue() == "!()"
				{
					AsNeed2 := iter->{BoxFuncBody^}

					for AsNeed2.MyFuncType.ParsCount
					{
						if AsNeed2.ItParams[it].ItName == name return AsNeed2.ItParams[it]
					}
				}
			}
		}
	}
	
	for SomeDef : DefsTable
	{
		if SomeDef != null
		if SomeDef.ItName == name 
		{
			return SomeDef->{Object^}
		}
	}
	
	return null
}



ReplaceNode := !(Object^ what, Object^ with) -> void
{
	wiEnd := with

	while wiEnd.Right != null 
	{
		wiEnd.Up = what.Up
		wiEnd = wiEnd.Right
	}
	wiEnd.Up = what.Up

	if what.Left == null
	{
		if what.Up != null
		{
			what.Up.Down = with
		}
		with.Left = null
	}else{
		with.Left = what.Left
		with.Left.Right = with
	}

	wiEnd.Right = what.Right
	if wiEnd.Right != null wiEnd.Right.Left = wiEnd

	with.SetUp(what.Up)

	what.Up = null
	what.Left = null
	what.Right = null
}

PopOutNode := !(Object^ what) -> void
{
	if what.Left != null
	{
		what.Left.Right = what.Right
		if what.Right != null	what.Right.Left = what.Left
	}else
	{
		if what.Up != null what.Up.Down = what.Right
		if what.Right != null what.Right.Left = null
	}
	what.Up = null
	what.Left = null
	what.Right = null
}


SomeId := int
GetNewId := !() -> int
{
	SomeId += 1
	return SomeId
}

