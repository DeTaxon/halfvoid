IsWord := !(string name) -> bool
{
	for c : name
	{
		if not (c in 'a'..'z' or c in 'A'..'Z' or c == '_') return false
	}
	return true
}

InsertParam := !(string name, Object^ ii , Queue.{ObjParam^} found,QueueSet.{int} Searched) -> void
{
	if ii is ObjParam
	{
		AsNeed := ii->{ObjParam^}
		if(AsNeed.GetName() == name)
		{
			found.Push(AsNeed) ; $temp	
		}
	}
}

CollectParamsAllByName := !(string name, Object^ start, Queue.{ObjParam^} found) -> void
{
	Searched := QueueSet.{int}()
	CollectParamsAllByName(name,start,found,Searched)
}

CollectParamsAllByName := !(string name, Object^ start, Queue.{ObjParam^} found, QueueSet.{int} Searched) -> void
{
	iterU := start
	LastPos := start
	while iterU != null
	{
		InsertParam(name,iterU,found,Searched)
		if iterU.Left != null 
		{
			iterU = iterU.Left 
		}else {
			iterU = iterU.Up
			
			if iterU?.Up? is BoxFile
			{	
				asN := iterU.Up->{BoxFile^}
				res := asN.VisibleParams.TryFind(name)
				if res != null{
					for res^ 
						found.Push(it) ; $temp
				}
				iterU = null
			}else{
				InsertParam(name,LastPos.Right[^],found,Searched)
				LastPos = iterU
			}
		}
	}

	for itCodeS,i : CodeSpaces ; $reverse
	{
		inMMap := itCodeS.codeParams.TryFind(name)
		if inMMap != null
		{
			InsertParam(name,inMMap^[^], found,Searched)
		}
	}
	
}


InsertFunc := !(string name, Object^ ii , Queue.{BoxFunc^} found, Queue.{BoxTemplate^} templates, bool IsSuffix,bool IsMethod,QueueSet.{int} Searched,bool IgnoreLibs) -> void
{
		if ii is ObjParam //ii.GetValue() == "i:=1"
		{
			AsNeed := ii->{ObjParam^}
			if(AsNeed.GetName() == name)
			{
				iterW := AsNeed.Down
				if iterW.GetValue() == "!()"
				{
					AsBoxFunc := iterW->{BoxFunc^}
					//if (AsBoxFunc.IsSuffix == SmeBug and not AsBoxFunc.IsVirtual)
					//	found.Push(AsBoxFunc)
					if IsSuffix
					{
						if AsBoxFunc.IsSuffix and not AsBoxFunc.IsVirtual
							found.Push(AsBoxFunc) ; $temp 
					}else{
						metApp := IsMethod == AsBoxFunc.IsMethod
						if name == "->{}" metApp = true
						if not IsWord(name) metApp = true
						if (not AsBoxFunc.IsSuffix) and (not (AsBoxFunc.IsVirtual and metApp))
							found.Push(AsBoxFunc) ; $temp
					}
				}else
				if iterW.GetValue() == "!(){}"
				{
					AsBoxFunc2 :=  iterW->{BoxTemplate^}
					if IsSuffix
					{
						if AsBoxFunc2.IsSuffix and AsBoxFunc2.IsVirtual
							templates.Push(AsBoxFunc2) ; $temp
					}else{
						if not (AsBoxFunc2.IsSuffix and (not (AsBoxFunc2.IsVirtual and IsMethod == AsBoxFunc2.IsMethod)))
							templates.Push(AsBoxFunc2) ; $temp
					}
				}
			}
		}else
		if ii.GetValue() == "{...}"
		{
			asC := ii->{BoxClass^}
			asC.GetWrappedFunc(name,found,templates)

		}
}


CollectFuncsByName := !(string name, Object^ start, Queue.{BoxFunc^} found, Queue.{BoxTemplate^} templates, bool IsSuffix,bool IsMethod,QueueSet.{int} Searched,bool IgnoreLibs) -> void
{
	iterU := start
	LastPos := start
	FirstMetClass := true

	if IgnoreLibs
	{
		iterr := start
		if iterr != null
		{
			while iterr.Up != null iterr = iterr.Up
			if FilesInSpace.Contain(iterr)
			{
				iterU = null
			}
		}

	}

	while iterU != null
	{
		lazy := true
		while lazy
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
			if iterU?.Up? is BoxFile
			{
				asN := iterU.Up->{BoxFile^}
				res := asN.VisibleParams.TryFind(name)
				if res != null{
					for res^ 
						InsertFunc(name,it,found,templates,IsSuffix,IsMethod,Searched,IgnoreLibs)
				}
				iterU = null
			}
		}
		if iterU != null
		{
			InsertFunc(name,iterU,found,templates,IsSuffix,IsMethod,Searched,IgnoreLibs)
			if iterU.Left != null and iterU.GetValue() != "{...}" 
			{
				iterU = iterU.Left 
			}else {
				iterU = iterU.Up

				InsertFunc(name,LastPos.Right[^],found,templates,IsSuffix,IsMethod,Searched,IgnoreLibs)
				LastPos = iterU
			}
		}
	}

}



FindFunc := !(string name, Object^ start, FuncInputBox itBox,bool IsMethod) -> BoxFunc^
{
	return FindStuff(name,start,itBox,false,IsMethod)
}
FindSuffix := !(string name, Object^ start,FuncInputBox itBox) -> BoxFunc^
{
	return FindStuff(name,start,itBox,true,false)
}
FindStuff := !(string name, Object^ start,FuncInputBox itBox, bool IsSuffix,bool IsMethod) -> BoxFunc^
{
	if itBox.itMetaPtr != null
		return null
		
	Searched := QueueSet.{int}()

	iterS := start
	if iterS != null
	{
		while iterS.Up != null
			iterS = iterS.Up
		Searched.Push(iterS->{BoxFile^}.fileId) ; $temp
	}

	iterr := start
	if iterr != null while iterr.Up != null iterr = iterr.Up

	Funcs := Queue.{BoxFunc^}() ; $temp
	Templs := Queue.{BoxTemplate^}() ; $temp
	CollectFuncsByName(name,start,Funcs,Templs,IsSuffix,IsMethod,Searched,true)

	func :=  GetBestFunc(itBox,Funcs,Templs)

	if func != null return func

	if ((not IsWord(name)) or IsMethod) and itBox.itPars.Size() != 0 // wtf if not word and size = 0
	{
		fT := itBox.itPars[0].first
		if fT? is TypeClass
		{
			asNeed := ((fT->{TypeClass^}).ToClass)
			funcRes := asNeed.GetFunc(name,itBox,false)
			if funcRes != null return funcRes
		}
	}

		
	Funcs.Clean()
	Templs.Clean()
	
	//if name == "."
	//{
		CollectFuncsByName(name,start,Funcs,Templs,IsSuffix,IsMethod,Searched,false)
		func3 := GetBestFunc(itBox,Funcs,Templs)
		if func3 != null return func3
	//}


	for itCodeS,ii : CodeSpaces ; $reverse
	{
		Funcs.Clean()
		Templs.Clean()
		inMMap := itCodeS.codeParams.TryFind(name)
		if inMMap != null
		{
			InsertFunc(name,inMMap^[^], Funcs, Templs,IsSuffix,IsMethod,Searched,false)
		}
		func4 := GetBestFunc(itBox,Funcs,Templs)
		if func4 != null return func4
	}

	inBuiltInMap := BuiltInFuncs.TryFind(name)
	if inBuiltInMap != null
	{
		Funcs.Push(inBuiltInMap^[^])
	}
	for it : Modules
	{
		fFunc := it.GetFunc(name)
		if fFunc != null
			Funcs.Push(fFunc->{BoxFunc^})
	}

	if BuiltInTemplates[^].FuncName == name
		Templs.Push(it) ; $temp

	func2 := GetBestFunc(itBox,Funcs,Templs)
	return func2

}
GetBestFunc := !(FuncInputBox itBox, Queue.{BoxFunc^} funcs, Queue.{BoxTemplate^} templs) -> BoxFunc^
{
	FoundC := funcs.Size()
	FoundT := templs.Size()

	if FoundC + FoundT == 0
	{
		return null
	}

	Priors := int^
	if FoundC != 0 Priors = new int[FoundC] else Priors = null ; $temp

	templsPrior := int^
	templsPrior = null
	if not templs.Empty() 
		templsPrior = new int[templs.Size()] ; $temp
	else templsPrior = null

	if FoundC != 0 and itBox.itPars.Size() == 0
	{
		if funcs[^].MyFuncType.ParsCount == 0
			return it
	}

	ComputePriors(funcs,itBox,Priors)
	for FoundC if Priors[it] == 0  { funcs[it].ParseBlock() return funcs[it] }

	for  itTmpl,i : templs
	{
		templsPrior[i] = itTmpl.GetPriority(itBox)
		if templsPrior[i] == 0 break
	}

	
	for FoundT if templsPrior[it] == 0 return templs[it].GetFunc(itBox)
	for FoundC if Priors[it] == 1  { funcs[it].ParseBlock() return funcs[it] }
	for FoundT if templsPrior[it] == 1 return templs[it].GetFunc(itBox)
	for FoundC if Priors[it] == 2  { funcs[it].ParseBlock() return funcs[it] }
	for FoundT if templsPrior[it] == 2 return templs[it].GetFunc(itBox)
	for FoundC if Priors[it] == 3  { funcs[it].ParseBlock() return funcs[it] }
	for FoundT if templsPrior[it] == 3 return templs[it].GetFunc(itBox)
	for FoundC if Priors[it] == 4  { funcs[it].ParseBlock() return funcs[it] }
	for FoundT if templsPrior[it] == 4 return templs[it].GetFunc(itBox)
	for FoundC if Priors[it] == 5 { funcs[it].ParseBlock() return funcs[it] }
	for FoundT if templsPrior[it] == 5 return templs[it].GetFunc(itBox)
	for FoundC if Priors[it] == 6  { funcs[it].ParseBlock() return funcs[it] }
	for FoundT if templsPrior[it] == 6 return templs[it].GetFunc(itBox)

	return null
}

ComputePriors := !(Queue.{BoxFunc^} Fun, FuncInputBox itBox , int^ priors) -> void
{
	for it,h : Fun
	{
		if it.IsSameConsts(itBox){
			priors[h] = ComputePriorFunc(it.MyFuncType,itBox)
		}
		else priors[h] = 255
		if priors[h] == 0 return void
	}
}

ComputePriorFunc := !(TypeFunc^ FuncTyp, FuncInputBox itBox) -> int
{	
	parsCount := itBox.itPars.Size()
	if parsCount == FuncTyp.ParsCount or (FuncTyp.IsVArgs and parsCount >= FuncTyp.ParsCount)
	{
		IsCorrect := true

		MaxPrior := 0

		for i : FuncTyp.ParsCount, par : itBox.itPars
		{
			SomePrior := 0
			if FuncTyp.ParsIsRef[i] 
			{
				if par.first != FuncTyp.Pars[i] SomePrior = 255
			}else {
				SomePrior = TypeCmp(par.first, FuncTyp.Pars[i])
			}
			if MaxPrior < SomePrior MaxPrior = SomePrior
		}
		return MaxPrior
	}
	return 255	
}



CmpSame := 0
CmpLoseless := 1
CmpLoselessItoF := 2 // Int to float
CmpLose := 3
CmpLoseFtoI := 5 // float to inr
CmpLosePtoFP := 6
CmpNoCmp := 255

TypeCmp := !(Type^ inType, Type^ funcType) -> int
{

	if inType == null return CmpNoCmp
	if funcType == null return CmpSame
	if inType == funcType return CmpSame

	if inType == GTypeFloat and funcType == GTypeDouble return CmpLoseless
	if inType == GTypeHalf and funcType == GTypeDouble return CmpLoseless
	if inType == GTypeHalf and funcType == GTypeFloat return CmpLoseless
	if (inType is TypeFatArr or inType is TypeArr)  and funcType is TypePoint
	{
		if inType.Base == funcType.Base
		{
			return CmpLoseless
		}
	}
	if funcType in ![GTypeDouble,GTypeFloat] and IsInt(inType) return CmpLoselessItoF 

	if inType is TypeStandart and funcType is TypeStandart
	{
		if IsInt(inType) and IsInt(funcType) {
			if GetIntSize(inType) < GetIntSize(funcType) return CmpLoseless
			return CmpLose
		}
	}
	//if (inType is TypePoint and funcType == GTypeBool) return 1
	//if inType is TypeFatArr and funcType == GTypeBool return 1

	if inType is TypePoint and inType.Base is TypeFunc  
		and funcType is TypePoint and funcType.Base is TypeFunc //func where?
	{
		return CmpLoseless
	}
	if inType is TypePoint and funcType is TypePoint
	{
		//if inType.Base.GetType() == "class" and funcType.Base.GetType() == "class"
		if inType.Base is TypeClass and funcType.Base is TypeClass
		{
			asClType1 := ((inType.Base)->{TypeClass^})
			asCl1 := asClType1.ToClass
			asClType2 := ((funcType.Base)->{TypeClass^})
			asCl2 := asClType2.ToClass

			while asCl1 != null
			{
				if asCl1 == asCl2 return CmpLoseless
				asCl1 = asCl1.Parent
			}

		}
		if funcType == VoidPType return CmpLose
		if inType == VoidPType and funcType != GTypeString return CmpLose
	}
	if inType is TypeClass //inType.GetType() == "class"
	{
		itFc := new FuncInputBox ; $temp 
		itFc.itPars.Emplace(inType,true)
		itFc.itConsts.Push(new ObjType(funcType)) ; $temp

		asN := inType->{TypeClass^}.ToClass
		if asN.GetFunc("->{}",itFc^,false) != null return CmpLose
	}

	if inType is TypeArr and funcType == GTypeVoidP return CmpLose
	if inType == GTypeDouble and funcType == GTypeFloat return CmpLose
	if inType == GTypeDouble and funcType == GTypeHalf return CmpLose
	if inType == GTypeFloat and funcType == GTypeHalf return CmpLose
	if inType == GTypeFloat and funcType == GTypeInt return CmpLoseFtoI
	if inType == GTypeDouble and funcType == GTypeInt return CmpLoseFtoI
	if inType == TypeTable[16] and funcType is TypeFatArr  return CmpLose

	if inType == GTypeVoidP and funcType is TypeFuncLambda return CmpLosePtoFP

	if inType == GTypeInt and funcType == GTypeBool return CmpLose

	//if inType.GetType() == "lambda" and funcType.GetType() == "lambda"
	if inType is  TypeFuncLambda and funcType is TypeFuncLambda 
	{
		asBI := ((inType.Base)->{TypeFunc^})
		asBO := ((inType.Base)->{TypeFunc^})

		if asBI.ParsCount == asBO.ParsCount
		{
			isGood := true
			for i : asBI.ParsCount
			{
				if asBI.Pars[i] != null
				{
					if asBI.Pars[i] != asBO.Pars[i]
						isGood = false
				}
			}
			if isGood return CmpLoseless
		}
		
	}
	
	return 255
}

