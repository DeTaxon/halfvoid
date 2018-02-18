
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

InsertFunc := !(string name, Object^ ii , Queue.{BoxFunc^} found, Queue.{BoxTemplate^} templates, bool IsSuffix) -> void
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
				if iterW.GetValue() == "d{}()"
				{
					AsBoxFunc2 :=  iterW->{BoxTemplate^}
					SomeBug := IsSuffix
						if (AsBoxFunc2.IsSuffix == SomeBug)
							templates.Push(AsBoxFunc2)
				}
			}
		}
}

CollectFuncsByName := !(string name, Object^ start, Queue.{BoxFunc^} found, Queue.{BoxTemplate^} templates, bool IsSuffix) -> void
{
	iterU := start
	LastPos := start
	while iterU != null
	{
		InsertFunc(name,iterU,found,templates,IsSuffix)
		if iterU.Left != null 
		{
			iterU = iterU.Left 
		}else {
			iterU = iterU.Up

			iterK := LastPos.Right
			while iterK != null
			{
				InsertFunc(name,iterK,found,templates,IsSuffix)
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
	Templs := Queue.{BoxTemplate^}()
	CollectFuncsByName(name,start,Funcs,Templs,IsSuffix)
	
	FoundC := Funcs.Size()
	FoundT := Templs.Size()

	if FoundC + FoundT == 0
	{
		return null //TODO:
	}

	Priors := int^
	if FoundC != 0 Priors = new int[FoundC] else Priors = null

	TemplsPrior := int^
	if not Templs.Empty() TemplsPrior = new int[Templs.Size()] else TemplsPrior = null

	//for FoundC Priors[it] = 255

	if pars.Empty()
	{
		for FoundC if Funcs[it].MyFuncType.ParsCount return Funcs[it]
	}

	ComputePriors(Funcs,pars,Priors)
	
	for FoundC if Priors[it] == 0 return Funcs[it]
	for FoundT if TemplsPrior[it] == 0 return Templs[it].GetFunc(pars)
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
	if funcType == null return 0
	if inType == funcType return 0

	if inType == GetType("float") and funcType == GetType("double") return 1
	if inType == GetType("int") and funcType == GetType("double") return 1

	if inType == GetType("double") and funcType == GetType("float") return 2

	if inType == GetType("int") and funcType == GetType("bool") return 3
	
	return 255
}


