
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
				{
					found.Push(iterW->{BoxFunc^})
				}
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
	
	FoundC := Funcs.Size()

	if FoundC == 0 return null //TODO:

	Priors := new int[FoundC]

	//for FoundC Priors[it] = 255

	ComputePriors(Funcs,pars,Priors)

	for FoundC if Priors[it] == 0 return Funcs[it]
	for FoundC if Priors[it] == 1 return Funcs[it]

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
			SomePrior := TypeCmp(iterT.Data, FuncTyp.Pars[i])
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
	
	return 255
}


