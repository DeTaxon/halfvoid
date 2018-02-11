
InsertParam := !(string name, Object^ ii , Queue.{ObjParam^} found) -> void
{
		if ii.GetValue() == "i:=1"
		{
			AsNeed := ii->{ObjParam^}
			if(AsNeed.GetName() == name)
			{
				found.Push(AsNeed)	
			}
		}
}

CollectParamsAllByName := !(string name, Object^ start, Queue.{ObjParam^} found) -> void
{
	iterU := start
	LastPos := start
	while iterU != null
	{
		InsertParam(name,iterU,found)
		if iterU.Left != null 
		{
			iterU = iterU.Left 
		}else {
			iterU = iterU.Up

			iterK := LastPos.Right
			while iterK != null
			{
				InsertParam(name,iterK,found)
				iterK = iterK.Right
			}
			LastPos = iterU
		}
	}
}

InsertFunc := !(string name, Object^ ii , Queue.{BoxFunc^} found, bool IsSuffix) -> void
{
		if ii.GetValue() == "i:=1"
		{
			AsNeed := ii->{ObjParam^}
			if(AsNeed.GetName() == name)
			{
				iterW := AsNeed.Down
				if iterW.GetValue() == "!()"
				{
					AsBoxFunc := iterW->{BoxFunc^}
					SomeBug := IsSuffix
					if (AsBoxFunc.IsSuffix == SomeBug)
						found.Push(AsBoxFunc)
				}
			}
		}
}

CollectFuncsByName := !(string name, Object^ start, Queue.{BoxFunc^} found, bool IsSuffix) -> void
{
	iterU := start
	LastPos := start
	while iterU != null
	{
		InsertFunc(name,iterU,found,IsSuffix)
		if iterU.Left != null 
		{
			iterU = iterU.Left 
		}else {
			iterU = iterU.Up

			iterK := LastPos.Right
			while iterK != null
			{
				InsertFunc(name,iterK,found,IsSuffix)
				iterK = iterK.Right
			}
			LastPos = iterU
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
	return FindStuff(name,start,pars,false)
}
FindSuffix := !(string name, Object^ start,Queue.{Type^} pars) -> BoxFunc^
{
	return FindStuff(name,start,pars,true)
}
FindStuff := !(string name, Object^ start,Queue.{Type^} pars, bool IsSuffix) -> BoxFunc^
{
	Funcs := Queue.{BoxFunc^}()
	CollectFuncsByName(name,start,Funcs,IsSuffix)
	
	FoundC := Funcs.Size()

	if FoundC == 0 
	{
		return null //TODO:
	}

	Priors := new int[FoundC]

	//for FoundC Priors[it] = 255

	if pars.Empty()
	{
		for FoundC if Funcs[it].MyFuncType.ParsCount return Funcs[it]
	}

	ComputePriors(Funcs,pars,Priors)
	
	for FoundC if Priors[it] == 0 return Funcs[it]
	for FoundC if Priors[it] == 1 return Funcs[it]
	for FoundC if Priors[it] == 2 return Funcs[it]
	for FoundC if Priors[it] == 3 return Funcs[it]

	return null
}

ComputePriors := !(Queue.{BoxFunc^} Fun, Queue.{Type^} pars, int^ priors) -> void
{
	iter := Fun.Start
	h := 0
	while iter != null
	{
		priors[h] = ComputePriorFunc(iter.Data,pars)
		h += 1	
		iter = iter.Next
	}
}

ComputePriorFunc := !(BoxFunc^ Fun, Queue.{Type^} pars) -> int
{
	parsCount := pars.Size()
	FuncTyp := Fun.MyFuncType
	if parsCount == FuncTyp.ParsCount or (FuncTyp.IsVArgs and parsCount >= FuncTyp.ParsCount)
	{
		IsCorrect := true
		iterT := pars.Start

		MaxPrior := 0

		for i : FuncTyp.ParsCount
		{
			SomePrior := 0
			if FuncTyp.ParsIsRef[i] 
			{
				if iterT.Data != FuncTyp.Pars[i] SomePrior = 255
			}else {
				SomePrior = TypeCmp(iterT.Data, FuncTyp.Pars[i])
			}
			if MaxPrior < SomePrior MaxPrior = SomePrior
			iterT = iterT.Next
		}
		return MaxPrior
	}
	return 255	
}



TypeCmp := !(Type^ inType, Type^ funcType) -> int
{
	if inType == funcType return 0

	if inType == GetType("float") and funcType == GetType("double") return 1
	if inType == GetType("int") and funcType == GetType("double") return 1

	if inType == GetType("double") and funcType == GetType("float") return 2

	if inType == GetType("int") and funcType == GetType("bool") return 3
	
	return 255
}


