
ContainTType := !(Object^ toCheck) -> bool
{
	res := Queue.{string}()
	return ContainTType(toCheck,res)
}
ContainTType := !(Object^ toCheck,Queue.{string} res) -> bool
{
	if toCheck == null 
		return false
	added := false
	bag := Stack.{Object^}() ; $temp
	bag.Push(toCheck)

	while bag.NotEmpty()
	{
		item := bag.Pop()
		if item.GetValue() == "~{}type" 
		{
			asNeed := item->{ObjTemplateType^}
			res.Push(asNeed.MyStr) ; $temp
			added = true
			//return true
		}
		if item.GetValue() == "..."
		{
			return false
		}
		bag.Push(item.Down[^])
	}
	return added
}


IsTemplate := !(Object^ sk) -> bool
{
	if sk == null return false

	Counter := 0

	for iter : sk.Down
	{
		if iter.GetValue() == ","
		{
			if Counter == 1 
			{
				return true
			}
			Counter = 0
		} else Counter += 1

		if iter is ObjData and iter.Down?.Right?.GetValue() == "..."
		{
			return true
		}

		if ContainTType(iter) return true //TODO: check after Syntax, not Before
	}
	if  Counter == 1 
	{
		
		itr := sk.Down
		if itr != null
		{
			while itr.Right != null
			{	
				itr = itr.Right
			}
			if itr.GetValue() == "..." return false
		}
		return true
	}


	if sk.Right?.GetValue() == "." and sk.Right.Right?.GetValue() == "{}"
	{
		for iter : sk.Right.Right.Down
		{
			if iter.GetValue() != ","
			{
				if iter.GetValue() == "..."
					return false

				if not iter.IsConst 
					if ParseType(iter) == null
						return true
				if ContainTType(iter) return true
			}
		}
	}

	return false
}

MiniWork := class
{
	SomeNode := Object^
	SomeType := Type^
	this := !(Object^ o, Type^ t) -> void
	{
		SomeNode = o
		SomeType = t
	}

	"=" := !(MiniWork ToSet) -> void
	{
		SomeNode = ToSet.SomeNode
		SomeType = ToSet.SomeType
	}
}

TemplateData := class 
{
	simpleFuncType := Type^
	ptrToFunc := BoxFunc^

	this := !() -> void {}
	this := !(Type^ toSetT,BoxFunc^ toSetF) -> void
	{
		simpleFuncType = toSetT
		ptrToFunc = toSetF
	}
	"=" := !(TemplateData toSet) -> void
	{
		simpleFuncType = toSet.simpleFuncType
		ptrToFunc = toSet.ptrToFunc
	}
}


BoxTemplate := class extend BoxFunc
{
	CopyParams := Object^
	CopyConsts := Object^
	CopyRet := Object^
	CopyTree := Object^

	EndPos := Object^

	NewFuncs := AVLMap.{int,Stack.{TemplateData}}

	FuncsTTemps := Queue.{Object^}
	TTNames := Queue.{string}

	TempReturnConsts := Queue.{ObjConstHolder^}^

	IsVirtual := bool

	GetCreatedFuncs := !() -> BoxFunc^[]
	{
		counter := 0
		for it2 : NewFuncs
		{
			counter += it2.Size()
		}
		if counter == 0
			return null->{BoxFunc^[]}
		i := 0
		retArr := new BoxFunc^[counter] ; $temp
		for it2 : NewFuncs
		{
			for it : it2
			{
				retArr[i] = it.ptrToFunc
				i += 1
			}
		}
		return retArr
	}

	CheckTypes := !(FuncInputBox itBox,Queue.{ObjConstHolder^} res,Type^^^ resTyps) -> bool
	{
		extraWalk := true
		re := true

		failedC := 0
		succedC := 0

		limit := 50
		if resTyps != null 
			resTyps^ = new Type^[itBox.itPars.Size()] ; $temp

		checkConsts := List.{Tuple.{Object^,Type^}}() ; $temp 
		//if ItConsts.Size() != null
		//{
		//	assert(ItConstsTT.Size() == itBox.itConsts.Size())
		//	for itC : ItConstsTT, itT : itBox.itConsts
		//	{
		//		if itC != null and itT is ObjType
		//		{
		//			checkConsts.Emplace(itC,itT->{ObjType^}.MyType)
		//		}
		//	}
		//}

		expectedChecks := FuncsTTemps.Size()
		if vargTypeBarrier != null
			expectedChecks += itBox.itPars.Size() - MyFuncType.ParsCount
		expectedChecks += checkConsts.Size()

		while expectedChecks != 0
		{
			failedC = 0
			succedC = 0

			metPars := false
			for fT : FuncsTTemps, i: 0, par : itBox.itPars
			{
				if i >= MyFuncType.ParsCount  {
					metPars = true
					break
				}
				if MyFuncType.Pars[i] == null
				{
					if fT != null
					{
						rTp := IsSameType(fT,par.first,res,re&)
						if resTyps != null resTyps^[i] = rTp
						if rTp == null
						{
							failedC++
						}else{
							succedC++
						}
						if not re 
						{
							return false
						}
					}else{
						succedC++
					}
				}else{
					succedC++
					failedC++
				}
			}
			for it : checkConsts
			{
				rTp := IsSameType(it.0,it.1,res,re&)
				if not re return false
				if rTp != null
				{
					succedC++
				}else{
					failedC++
				}
			}
			if vargTypeBarrier != null
			{
				for par,i : itBox.itPars
				{
					if i < MyFuncType.ParsCount continue

					rTp := IsSameType(vargTypeBarrier.Up.Down,par.first,res,re&)

					if resTyps != null resTyps^[i] = rTp
					if not re return false
					if rTp == null{
						failedC++
					}else{
						succedC++
					}
				}
			}
			if (metPars and succedC == MyFuncType.ParsCount) or  succedC == expectedChecks
			{
				if extraWalk {
					extraWalk = false
					continue
				}
				break
			}
			if failedC == expectedChecks{
				printf("status %i %i %i\n",succedC,failedC,expectedChecks)
				return false
			}
			limit -= 1
			if limit <= 0 {
				printf("status %i %i %i\n",succedC,failedC,expectedChecks)
				printf("types\n")
				printf("typ %s\n",itBox.itPars[^].first.GetGoodName())
				EmitError("compiler error")
				return false
			}
		}
		if CopyConsts != null
		{
			return IsEqConsts(CopyConsts,itBox,res)
		}
		return true
	}
	GetItem := virtual !(string name) -> Object^
	{
		if TempReturnConsts != null
		{
			if TempReturnConsts^[^].ItName == name
				return it.Down
		}
		return null
	}

	CheckBottomParams := !(Object^ firstNon) -> void
	{
		if firstNon == vargTypeBarrier?.Up or firstNon == vargTypeBarrier?.Up?.Up
			return void
		if ContainTType(firstNon,TTNames)
		{
			FuncsTTemps.Push(firstNon)
		}else{
			FuncsTTemps.Push(null->{Object^})
		}
	}
	this := !(Object^ inPars, Object^ inOutType, Object^ cons, bool RetRef, string SomeName, Object^ Stuf,bool IsSuf, Type^ metC, bool IsVirt,bool isRetSelf,Object^ anc) -> void
	{
		Up = anc.Up
		IsSelfReturn = isRetSelf
		IsRetRef = RetRef
		IsVirtual = IsVirt
		FuncName = SomeName
		MethodType = metC
		NewFuncs."this"()
		Line = inPars.Line //TODO: replace with good line

		
		if inPars != null {
			CopyParams = inPars.Clone()
			CopyParams.SetUp(this&)
		}
		if cons != null
		{
			CopyConsts = cons//.Clone()
			ParseConsts(CopyConsts)
			CopyConsts.Up = this&
			MakeGoodConsts(CopyConsts)
		}
		if inOutType != null {
			CopyRet = inOutType.Clone()
			CopyRet.SetUp(this&)
		}
		if Stuf != null CopyTree = Stuf//.Clone()

		IsSuffix = IsSuf
		ParseParams(CopyParams,CopyRet,true)

		SyntaxCompress(CopyParams.Down,PriorityData)
		iter := CopyParams.Down
		firstNon := Object^()
		//FuncsTs := Queue.{Object^}() 

		if FuncName != "new" and FuncName != "delete" and FuncName != "~For" and metC != null
			FuncsTTemps.Push(null->{Object^})

		while iter != null
		{
			if iter.GetValue() == ","
			{
				CheckBottomParams(firstNon)
				firstNon = null
			}else{
				if firstNon == null 
				{
					firstNon = iter
				}
			}
			iter = iter.Right
			if iter == null
			{
				CheckBottomParams(firstNon)
				firstNon = null
			}else{
				if firstNon == null 
				{
					firstNon = iter
				}
			}
		}
		if not IsWord(SomeName) IsSuffix = false

		if metC != null and metC is TypeClass
		{
			hldr := GetBoxClassFuncsHolder(anc)
			assert(hldr != null)
			hldr.templates[FuncName].Push(this&)
		}
	}
	GetPriority :=virtual !(FuncInputBox itBox) -> int
	{
		parsCount := itBox.itPars.Size()
		FType := MyFuncType

		if itBox.itConsts.Size() != ItConsts.Size() 
			return 255

		st := Queue.{ObjConstHolder^}()
		if not CheckTypes(itBox,st,null) 
			return 255

		//if CopyRet != null
		//{
		//	//printf("hoh %s\n",FuncName)
		//	itT := ParseType(CopyRet,null,st&)
		//	if itT == null {
		//		//printf("heh %s %s %i\n",st[^].ItName,it.Down.GetValue(),st.Size())
		//		return 255
		//	}
		//}
		
		if parsCount == FType.ParsCount or (FType.IsVArgs and parsCount >= FType.ParsCount) or (vargsName != null and parsCount >= FType.ParsCount)
		{
			IsCorrect := true

			MaxPrior := 0

			for i : FType.ParsCount, par : itBox.itPars
			{
				SomePrior := 0
			
				if FType.Pars[i] != null and (FuncsTTemps.Size() >= i or FuncsTTemps[i] == null)
				{
					if FType.ParsIsRef[i]
					{
						if par.first != FType.Pars[i] SomePrior = 255
					}else {
						SomePrior = TypeCmp(par.first, FType.Pars[i])
					}
					if MaxPrior < SomePrior MaxPrior = SomePrior
				}
			}
			return MaxPrior
		}
		return 255	
	}
	CreateFuncPointer := virtual !(FuncInputBox itBox) -> TypeFunc^
	{
		outT := Queue.{Type^}() ; $temp
		FType := MyFuncType
		
		for it : FType.ParsCount, par : itBox.itPars
		{
			if FType.Pars[it] == null
			{
				outT.Push(par.first)
			}else{
				outT.Push(FType.Pars[it])
			}
		}

		resTp := Type^^
		parConsts := Queue.{ObjConstHolder^}()
		CheckTypes(itBox,parConsts,resTp&)

		for it,i : outT
		{
			if resTp[i] != null
				it = resTp[i]
		}

		newRet := MyFuncType.RetType
		retRefArray := MyFuncType.ParsIsRef

		if MyFuncType.RetType == null and CopyRet != null
		{
			if vargsName != null 
			{
				extrVal2 := AVLMap.{char^,Type^}()
				bstType := Type^()
				badType := false
				ii := funcUserParamsCount
				while ii < itBox.itPars.Size()
				{
					if bstType == null bstType = itBox.itPars[ii].first
					else bstType = TypeFight(bstType,itBox.itPars[ii].first)
					if bstType == null badType = true
					ii++
				}
				if not badType extrVal2[vargsName] = bstType ; $temp
				newRet = ParseType(CopyRet,null,null,extrVal2&)
			}else{
				newRet = ParseType(CopyRet,null,null,null)
			}
		}
		if itBox.itPars.Size() > FType.ParsCount
		{
			retRefArray = new bool[itBox.itPars.Size()] ; $temp 
			for i : itBox.itPars.Size()
				retRefArray[i] = MyFuncType.ParsIsRef[i]
			for it, i : itBox.itPars
			{
				if i < outT.Size() continue
				preSet := it.first
				if resTp[i] != null
					preSet = resTp[i]
				outT.Push(preSet)
			}
		}
		//if CopyRet != null
		//{
		//	itT := ParseType(CopyRet,null,parConsts&)
		//	newRet = itT
		//}
		return GetFuncType(outT,retRefArray,newRet,MyFuncType.RetRef,MyFuncType.IsVArgs)
	}
	GetFunc := virtual !(FuncInputBox itBox) -> BoxFunc^
	{
		getTempData = itBox&
		defer getTempData = null

		outT := Queue.{Type^}() ; $temp

		parConsts := Queue.{ObjConstHolder^}()
		CheckTypes(itBox,parConsts,null)

		TempReturnConsts = parConsts& //TODO: replace
		newFuncType := CreateFuncPointer(itBox)
		TempReturnConsts = null

		if newFuncType == null return null

		cmpFuncF := newFuncType
		if cmpFuncF.RetType != null cmpFuncF = ExchangeFuncType(cmpFuncF,null)

		somePos := 0

		sHash := cmpFuncF.ItHash + itBox.GetConstsHash()
		toAddLine := ref NewFuncs[sHash]
		
		for it : toAddLine
		{
			if it.simpleFuncType  == cmpFuncF
			{
				if it.ptrToFunc.IsSameConsts(itBox) 
					return it.ptrToFunc
			}
			somePos += 1
		}
		newFunc := GetNewFunc(itBox,newFuncType)
		assert(newFunc != null)
		if IsSelfReturn
			newFunc.IsSelfReturn = true
		if IsPassAttrs
			newFunc.IsPassAttrs = true

		newFunc.ItVals.Push(parConsts[^])
		newFunc.ItConsts.Push(itBox.itConsts[^])
		newFunc.ItAttrs[key] = itBox.itAttrs[^key]
		
		if newFunc == null return null

		WorkBag.Push(newFunc,State_Start)

		toAddLine.Emplace(cmpFuncF,newFunc)

		if EndPos == null
		{
			newFunc.Up = this&
			Down = newFunc
			EndPos = Down
		}else{
			EndPos.Right = newFunc
			newFunc.Left = EndPos
			EndPos = newFunc
			EndPos.Up = this&
		}
		return newFunc	
	}
	GetNewFunc := virtual !(FuncInputBox itBox, TypeFunc^ FunType) -> BoxFunc^
	{

		newFunc := new BoxFuncBody(MyFuncParamNames,FunType,FuncName,CopyTree.Clone(),IsSuffix,MethodType,IsVirtual)
		newFunc.ParseBlock()
		newFunc.IsSelfReturn = IsSelfReturn
		if MyFuncType != null newFunc.funcUserParamsCount = MyFuncType.ParsCount
		newFunc.vargsName = vargsName
		newFunc.CountAttrs = true
		newFunc.Line = Line
		newFunc.IsMethod = IsMethod

		return newFunc	
	}
	PrintGlobal := virtual !(TIOStream f) -> void
	{
		Down[^].PrintGlobal(f)
	}

	DoTheWork := virtual !(int pri) -> void
	{
	}
	GetValue := virtual !() -> string
	{
		return "!(){}"
	}
}
