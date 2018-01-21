
GetItem := !(string name, Object^ start) -> Object^
{
	iter := start

	while iter != null
	{
		if iter.GetValue() == "i:=1"
		{
			AsNeed := iter->{ObjParam^}
			if (AsNeed.MyStr == name) {
				if not AsNeed.AskedGetUse WorkBag.Push(iter,State_GetUse)
				return iter
			}
		}
		if iter.Left != null iter = iter.Left else iter = iter.Up
	}
	
	for SomeDef : DefsTable
	{
		if SomeDef != null
		if SomeDef.ItName == name 
		{
			//printf("found %s\n", SomeDef.GetValue())
			return SomeDef->{Object^}
		}
	}
	
	return null
}

CollectFuncsByName := !(string name, Object^ start, Queue.{BoxFunc^} found) -> void
{
	iterU := start
	while iterU != null
	{
		if iterU.GetValue() == "i:=1"
		{
			AsNeed := iterU->{ObjParam^}
			if(AsNeed.GetName() == name)
			{
				iterW := iterU.Down
				if iterW.GetValue() == "!()"
					found.Push(iterW->{BoxFunc^})
			}
		}
		if iterU.Left != null 
		{
			iterU = iterU.Left 
		}else {
			iterU = iterU.Up
		}
	}

	iterQ := BuiltInFuncs.Start
	while iterQ != null
	{
		if iterQ.Data.FuncName == name
		{
			found.Push(iterQ.Data)
		}
		iterQ = iterQ.Next
	}
}

FindFunc := !(string name, Object^ start,Queue.{Type^} pars) -> BoxFunc^
{
	Funcs := Queue.{BoxFunc^}()
	CollectFuncsByName(name,start,Funcs)

	May := BoxFunc^

	May = RelaxedSearch(name,Funcs,pars)
	if May != null return May

	return May
}

RelaxedSearch := !(string name,Queue.{BoxFunc^} Fun, Queue.{Type^} pars) -> BoxFunc^
{
	iter := Fun.Start
	parsCount := pars.Size()
	while iter != null
	{
		FuncTyp := iter.Data.MyFuncType
		if parsCount == FuncTyp.ParsCount or (FuncTyp.IsVArgs and parsCount >= FuncTyp.ParsCount)
		{
			IsCorrect := true
			iterT := pars.Start

			for i : FuncTyp.ParsCount
			{
				IsCorrect = IsCorrect and FuncTyp.Pars[i] == iterT.Data
				iterT = iterT.Next
			}
			if IsCorrect return iter.Data
		}
		iter = iter.Next
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
		with.Up.Down = with	
	}else{
		with.Left = what.Left
		with.Left.Right = with
	}

	wiEnd.Right = what.Right
	if wiEnd.Right != null wiEnd.Right.Left = wiEnd
}

PopOutNode := !(Object^ what) -> void
{
	if what.Left != null
	{
		what.Left.Right = what.Right
		if what.Right != null
			what.Right.Left = what.Left
	}else
	{
		what.Up.Down = what.Right
		if what.Right != null
			what.Right.Left = null
	}
}


SomeId := int
GetNewId := !() -> int
{
	SomeId += 1
	return SomeId
}

