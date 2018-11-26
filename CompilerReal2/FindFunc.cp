#import "Tree.cp"
#import "Globals.cp"
#import "FuncInputBox.cp"

IsWord := !(string name) -> bool
{
	for c : name
	{
		if not (c in 'a'..'z' or c in 'A'..'Z' or c == '_') return false
	}
	return true
}

InsertParam := !(string name, Object^ ii , Queue.{ObjParam^} found,Queue.{int} Searched) -> void
{
	if ii.GetValue() == "i:=1"
	{
		AsNeed := ii->{ObjParam^}
		if(AsNeed.GetName() == name)
		{
			found.Push(AsNeed)	
		}
	}
	if ii.GetValue() == "#import cp"
	{
		asNeed := ii->{ImportCmd^}
		fl := asNeed.GetFile()

		if fl != null
		{

			Found := false

			for Searched
			{
				if it == fl.fileId
				{
					Found = true
					break
				}
			}
			if not Found
			{
				Searched.Push(fl.fileId)
				CollectParamsAllByName(name,fl.Down,found,Searched)
			}
		}
	}
}

CollectParamsAllByName := !(string name, Object^ start, Queue.{ObjParam^} found) -> void
{
	Searched := Queue.{int}()
	CollectParamsAllByName(name,start,found,Searched)
}

CollectParamsAllByName := !(string name, Object^ start, Queue.{ObjParam^} found, Queue.{int} Searched) -> void
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

			iterK := LastPos.Right
			while iterK != null
			{
				InsertParam(name,iterK,found,Searched)
				iterK = iterK.Right
			}
			LastPos = iterU
		}
	}

	for lb : ForcedLibs
	{
		Found := false
		for sd : Searched{
			if lb.fileId == sd {
				Found = true
				break
			}
		}
		if not Found
		{
			Searched.Push(lb.fileId)
			CollectParamsAllByName(name,lb.Down,found,Searched)
		}
	}
}


InsertFunc := !(string name, Object^ ii , Queue.{BoxFunc^} found, Queue.{BoxTemplate^} templates, bool IsSuffix,bool IsMethod,Queue.{int} Searched,bool IgnoreLibs) -> void
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
					//if (AsBoxFunc.IsSuffix == SmeBug and not AsBoxFunc.IsVirtual)
					//	found.Push(AsBoxFunc)
					if SomeBug
					{
						if AsBoxFunc.IsSuffix and not AsBoxFunc.IsVirtual
							found.Push(AsBoxFunc)
					}else{
						metApp := IsMethod == AsBoxFunc.IsMethod
						if name == "->{}" metApp = true
						if not IsWord(name) metApp = true
						if not AsBoxFunc.IsSuffix and not AsBoxFunc.IsVirtual and metApp
							found.Push(AsBoxFunc)
					}
				}
				if iterW.GetValue() == "!(){}"
				{
					AsBoxFunc2 :=  iterW->{BoxTemplate^}
					SomeBug := IsSuffix
					if SomeBug
					{
						if AsBoxFunc2.IsSuffix and AsBoxFunc2.IsVirtual
							templates.Push(AsBoxFunc2)
					}else{
						if not AsBoxFunc2.IsSuffix and not AsBoxFunc2.IsVirtual and IsMethod == AsBoxFunc2.IsMethod
							templates.Push(AsBoxFunc2)
					}
				}
			}
		}
		if ii.GetValue() == "{...}"
		{
			//if name == "."{	
			//	asC := ii->{BoxClass^}
			//	templates.Push((asC.UnrollTemplate)->{BoxTemplate^})
			//}
			asC := ii->{BoxClass^}
			asC.GetWrappedFunc(name,found,templates)

		}
		if ii.GetValue() == "#import cp"
		{
			asNeed := ii->{ImportCmd^}
			fl2 := asNeed.GetFile()
			fl3 := fl2->{Object^}

			Found := false

			for Searched
			{
				if it == fl2.fileId{
					Found = true
					break
				}
			}
			if not Found
			{
				Searched.Push(fl2.fileId)
				CollectFuncsByName(name,fl3.Down,found,templates,IsSuffix,IsMethod,Searched,IgnoreLibs)
			}
		}
}


CollectFuncsByName := !(string name, Object^ start, Queue.{BoxFunc^} found, Queue.{BoxTemplate^} templates, bool IsSuffix,bool IsMethod,Queue.{int} Searched,bool IgnoreLibs) -> void
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
			for it : ForcedLibs
			{
				if it == iterr iterU = null
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
		}
		if iterU != null
		{
			InsertFunc(name,iterU,found,templates,IsSuffix,IsMethod,Searched,IgnoreLibs)
			if iterU.Left != null 
			{
				iterU = iterU.Left 
			}else {
				iterU = iterU.Up

				iterK := LastPos.Right
				while iterK != null
				{
					InsertFunc(name,iterK,found,templates,IsSuffix,IsMethod,Searched,IgnoreLibs)
					iterK = iterK.Right
				}
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
	Searched := Queue.{int}()

	iterS := start
	if iterS != null
	{
		while iterS.Up != null
			iterS = iterS.Up
		Searched.Push(iterS->{BoxFile^}.fileId)
	}

	iterr := start
	if iterr != null while iterr.Up != null iterr = iterr.Up

	Funcs := Queue.{BoxFunc^}()
	Templs := Queue.{BoxTemplate^}()
	CollectFuncsByName(name,start,Funcs,Templs,IsSuffix,IsMethod,Searched,true)

	func :=  GetBestFunc(itBox,Funcs,Templs)
	if func != null return func

	if (not IsWord(name) or IsMethod) and itBox.itPars.Size() != 0 // wtf if not word and size = 0
	{
		fT := itBox.itPars[0].first
		if fT != null
		if fT.GetType() == "class"
		{
			asNeed := ((fT->{TypeClass^}).ToClass)
			funcRes := asNeed.GetFunc(name,itBox,false)
			if funcRes != null return funcRes
		}
	}

		
	Funcs.Clean()
	Templs.Clean()
	for ForcedLibs
	{
		itUp := (it->{Object^}).Down
		if it == iterr itUp = start
		CollectFuncsByName(name,itUp,Funcs,Templs,IsSuffix,IsMethod,Searched,false)
	}
	for itQ : BuiltInFuncs
	{
		if itQ.FuncName == name
		{
			Funcs.Push(itQ)
		}
	}

	for itH : BuiltInTemplates
	{
		if itH.FuncName == name
		{
			Templs.Push(itH)
		}
	}
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
	if FoundC != 0 Priors = new int[FoundC] else Priors = null

	templsPrior := int^
	templsPrior = null
	if not templs.Empty() templsPrior = new int[templs.Size()] else templsPrior = null

	if FoundC != 0 and itBox.itPars.Size() == 0
	{
		for funcs
		{
			if it.MyFuncType.ParsCount == 0 
			{
				return it
			}
		}
	}
	//for it : FoundC, fFuncs : funcs
	//{
	//	for FoundC if fFuncs.MyFuncType.ParsCount return fFuncs
	//}

	for  itTmpl : templs, it : 0
	{
		templsPrior[it] = itTmpl.GetPriority(itBox)
	}

	ComputePriors(funcs,itBox,Priors)
	
	for FoundC if Priors[it] == 0  { funcs[it].ParseBlock() return funcs[it] }
	for FoundT if templsPrior[it] == 0 return templs[it].GetFunc(itBox)
	for FoundC if Priors[it] == 1  { funcs[it].ParseBlock() return funcs[it] }
	for FoundT if templsPrior[it] == 1 return templs[it].GetFunc(itBox)
	for FoundC if Priors[it] == 2  { funcs[it].ParseBlock() return funcs[it] }
	for FoundT if templsPrior[it] == 2 return templs[it].GetFunc(itBox)
	for FoundC if Priors[it] == 3  { funcs[it].ParseBlock() return funcs[it] }
	for FoundT if templsPrior[it] == 3 return templs[it].GetFunc(itBox)

	return null
}

ComputePriors := !(Queue.{BoxFunc^} Fun, FuncInputBox itBox , int^ priors) -> void
{
	for it : Fun, h : 0
	{
		if it.IsSameConsts(itBox)
			priors[h] = ComputePriorFunc(it.MyFuncType,itBox)
		else priors[h] = 255
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



TypeCmp := !(Type^ inType, Type^ funcType) -> int
{

	if inType == null return 255
	if funcType == null return 0
	if inType == funcType return 0

	if inType == GetType("float") and funcType == GetType("double") return 1
	if inType == GetType("int") and funcType == GetType("double") return 1
	if inType.GetType() == "fatarr"  and funcType.GetType() == "point" 
	{
		if inType.Base == funcType.Base
		{
			return 1
		}
	}
	if inType.GetType() == "arr"  and funcType.GetType() == "point" 
	{
		if inType.Base == funcType.Base
		{
			return 1
		}
	}

	if inType.GetType() == "standart" and funcType.GetType() == "standart"
	{
		if IsInt(inType) and IsInt(funcType) {
			if GetIntSize(inType) < GetIntSize(funcType) return 1
			return 2
		}
	}
	if (inType.GetType() == "point" and funcType == GetType("bool")) return 1
	if inType.GetType() == "fatarr" and funcType == GetType("bool") return 1

	if inType.GetType() == "point" and funcType.GetType() == "point"
	{
		if inType.Base.GetType() == "class" and funcType.Base.GetType() == "class"
		{
			asClType1 := ((inType.Base)->{TypeClass^})
			asCl1 := asClType1.ToClass
			asClType2 := ((funcType.Base)->{TypeClass^})
			asCl2 := asClType2.ToClass

			while asCl1 != null
			{
				if asCl1 == asCl2 return 1
				asCl1 = asCl1.Parent
			}

		}
		if funcType == VoidPType return 2
		if inType == VoidPType and funcType != GetType("string") return 2
	}


	if inType == GetType("double") and funcType == GetType("float") return 2
	if inType == TypeTable[16] and funcType.GetType() == "fatarr" return 2

	if inType == GetType("int") and funcType == GetType("bool") return 3

	if inType.GetType() == "lambda" and funcType.GetType() == "lambda"
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
			if isGood return 0
		}
		
	}
	
	return 255
}



//AddClassFuncs := !(string name, BoxClass^ cl, void^ funcsP, void^ templsP) -> void
//{
//	funcs := funcsP->{Queue.{BoxFunc^}^}
//	templs := templsP->{Queue.{BoxTemplate^}i^}
//	if not cl.createdWrappers
//	{
//		cl.createdWrappers = true
//
//		iter := cl.Down.Down
//		while iter != null
//		{
//			if iter.GetValue() == "i:=1" and iter.Down != null
//			{
//				if iter.Down.GetValue() == "!()"
//				{
//					cl.FuncWrappers.Push(new BuiltInTemplateFuncWrapper((iter.Down)->{BoxFunc^},cl))
//				}
//			}
//			iter = iter.Right
//		}
//	}
//
//	for i : cl.FuncWrappers.Size()
//	{
//		if cl.FuncWrappers[i].FuncName == name
//			funcs.Push(cl.FuncWrappers[i])
//	}
//}

