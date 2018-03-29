
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
				if iterW.GetValue() == "!(){}"
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
	FirstMetClass := true

	while iterU != null
	{
		lazy := true
		while (lazy)
		{
			lazy = iterU.Up != null
			if lazy
			{
				lazy = iterU.Up.GetValue() == "{d}"
				if lazy lazy = iterU.Up.Up != null
				if lazy lazy = iterU.Up.Up.GetValue() == "{...}"
				if lazy and FirstMetClass
				{
					asClass := ((iterU.Up.Up)->{BoxClass^})
					theTemplate := BoxTemplate^
					if (name == ".") {
						templates.Push((asClass.AutoFieldTemplate)->{BoxTemplate^})
						
					}else{
						//AddClassFuncs(name,asClass,found&,templates&)
					}
					FirstMetClass = false
				}
			}
			if lazy
			{
				iterU = iterU.Up
				LastPos = iterU
			}
		}
		if iterU != null
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

	iterH := BuiltInTemplates.Start
	while iterH != null
	{
		if iterH.Data.FuncName == name
		{
			templates.Push(iterH.Data)
		}
		iterH = iterH.Next
	}
}



FindFunc := !(string name, Object^ start,Queue.{Type^} pars) -> BoxFunc^
{
	wut := Queue.{Object^}()
	return FindFunc(name,start,pars,wut)
}
FindFunc := !(string name, Object^ start,Queue.{Type^} pars,Queue.{Object^} consts) -> BoxFunc^
{
	return FindStuff(name,start,pars,consts,false)
}
FindSuffix := !(string name, Object^ start,Queue.{Type^} pars) -> BoxFunc^
{
	return FindStuff(name,start,pars,true)
}
FindStuff := !(string name, Object^ start,Queue.{Type^} pars, bool IsSuffix) -> BoxFunc^
{
	wut := Queue.{Object^}()
	return FindStuff(name,start,pars,wut,IsSuffix)
}
FindStuff := !(string name, Object^ start,Queue.{Type^} pars,Queue.{Object^} consts, bool IsSuffix) -> BoxFunc^
{
	Funcs := Queue.{BoxFunc^}()
	Templs := Queue.{BoxTemplate^}()
	CollectFuncsByName(name,start,Funcs,Templs,IsSuffix)
	return GetBestFunc(pars,consts,Funcs,Templs)
}
GetBestFunc := !(Queue.{Type^} pars, Queue.{BoxFunc^} funcs, Queue.{BoxTemplate^} templs) -> BoxFunc^
{
	wut := Queue.{Object^}()
	return GetBestFunc(pars,wut,funcs,templs)
}
GetBestFunc := !(Queue.{Type^} pars,Queue.{Object^} consts, Queue.{BoxFunc^} funcs, Queue.{BoxTemplate^} templs) -> BoxFunc^
{
	FoundC := funcs.Size()
	FoundT := templs.Size()

	if FoundC + FoundT == 0
	{
		return null //TODO:
	}

	Priors := int^
	if FoundC != 0 Priors = new int[FoundC] else Priors = null

	templsPrior := int^
	templsPrior = null
	if not templs.Empty() templsPrior = new int[templs.Size()] else templsPrior = null

	//for FoundC Priors[it] = 255

	if pars.Empty()
	{
		for FoundC if funcs[it].MyFuncType.ParsCount return funcs[it]
	}

	for FoundT
	{
		templsPrior[it] = templs[it].GetPriority(pars,consts)
	}

	ComputePriors(funcs,pars,Priors)
	
	for FoundC if Priors[it] == 0 return funcs[it]
	for FoundT if templsPrior[it] == 0 return templs[it].GetFunc(pars,consts)
	for FoundC if Priors[it] == 1 return funcs[it]
	for FoundT if templsPrior[it] == 1 return templs[it].GetFunc(pars,consts)
	for FoundC if Priors[it] == 2 return funcs[it]
	for FoundT if templsPrior[it] == 2 return templs[it].GetFunc(pars,consts)
	for FoundC if Priors[it] == 3 return funcs[it]
	for FoundT if templsPrior[it] == 3 return templs[it].GetFunc(pars,consts)


	return null
}

ComputePriors := !(Queue.{BoxFunc^} Fun, Queue.{Type^} pars, int^ priors) -> void
{
	iter := Fun.Start
	h := 0
	while iter != null
	{
		priors[h] = ComputePriorFunc(iter.Data.MyFuncType,pars)
		h += 1	
		iter = iter.Next
	}
}

ComputePriorFunc := !(TypeFunc^ FuncTyp, Queue.{Type^} pars) -> int
{
	parsCount := pars.Size()
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
	//if (inType.GetType() == "point" and funcType == (GetType("void").GetPoint())) return 2
	//if (funcType.GetType() == "point" and inType == (GetType("void").GetPoint())) return 2
	if (funcType.GetType() == "point" and inType == VoidPType) return 2
	if (inType.GetType() == "point" and funcType == VoidPType) return 2

	if inType == GetType("int") and funcType == GetType("bool") return 3
	
	return 255
}



AddClassFuncs := !(string name, BoxClass^ cl, void^ funcsP, void^ templsP) -> void
{
	funcs := funcsP->{Queue.{BoxFunc^}^}
	templs := templsP->{Queue.{BoxTemplate^}i^}
	if not cl.createdWrappers
	{
		cl.createdWrappers = true

		iter := cl.Down.Down
		while iter != null
		{
			if iter.GetValue() == "i:=1" and iter.Down != null
			{
				if iter.Down.GetValue() == "!()"
				{
					cl.FuncWrappers.Push(new BuiltInTemplateFuncWrapper((iter.Down)->{BoxFunc^},cl))
				}
			}
			iter = iter.Right
		}
	}

	for i : cl.FuncWrappers.Size()
	{
		if cl.FuncWrappers[i].FuncName == name
			funcs.Push(cl.FuncWrappers[i])
	}
}

