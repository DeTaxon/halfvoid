ParseFuncDataR := !(Object^ item) -> Object^
{
	iter := item.Down

	if iter == null return null

	IsStatic := false
	IsVirtual := false

	ClassPtr := BoxClass^
	ClassType := Type^
	ClassPtr = null
	ClassType = null

	RetT := Object^
	RetT = null
	RetRef := false
	itsSelfRet := false

	constsI := null->{Object^}

	itLine := item.Line


	if iter.GetValue() == "virtual"
	{
		IsVirtual = true
		iter = iter.Right
	}
	if iter.GetValue() == "static"
	{
		IsStatic = true
		iter = iter.Right
	}


	if iter.GetValue() != "!" return null

	ExtraIter := item
	while ExtraIter != null
	{
		if ExtraIter.GetValue() == "{...}"
		{
			ClassPtr = ExtraIter->{BoxClass^}
			ClassType = ClassPtr.ClassType
			ExtraIter = null
		}else 	ExtraIter = ExtraIter.Up
	}

	iter = iter.Right
	
	if iter.GetValue() != "()" return null

	ParamsObj := iter
	iter = iter.Right

	if iter.GetValue() == "."
	{
		constsI = iter.Right
		iter = constsI.Right
	}

	if iter.GetValue() == "->"
	{
		iter = iter.Right
		if iter.GetValue() == "ref"
		{
			RetRef = true
			iter = iter.Right
		}
		RetT = iter
		iter = iter.Right
	}
	if iter.GetValue() == "self_return"
	{
		iter = iter.Right
		itsSelfRet = true
		if ClassType == null
			return null
	}


	iterForName := iter.Up
	FName := ""
	IsSuf := false

	while iterForName != null
	{
		if iterForName.GetValue() == "i:=1"
		{
			dynCa := iterForName->{ObjParam^}
			IsSuf = dynCa.IsStrName
			FName = dynCa.MyStr

			if FName == "![]" IsSuf = false
			if FName == "this" IsSuf = false

			if IsOper(FName) IsSuf = false

			iterForName = null
		}else	iterForName = iterForName.Up
	}
	if constsI != null	SyntaxCompress(constsI,PriorityData)
	if iter.GetValue() == "declare"
	{
		return new BoxFuncDeclare(ParamsObj,RetT,FName)
	}
	if iter.GetValue() == "{}"
	{
		maybeForceConsts := constsI != null 
		if maybeForceConsts maybeForceConsts =  constsI.Down == null
		if maybeForceConsts or IsTemplate(ParamsObj)
		{
			preRes := new BoxTemplate(ParamsObj,RetT,constsI,RetRef,FName,iter,IsSuf,ClassType,IsVirtual,itsSelfRet)
			preRes.IsSelfReturn = itsSelfRet
			preRes.Line = itLine
			return preRes
		}
		preRet := new BoxFuncBody(ParamsObj,RetT,constsI,RetRef,FName,iter,IsSuf,ClassType,IsVirtual,itsSelfRet)
		preRet.IsSelfReturn = itsSelfRet
		preRet.Line = itLine
		return preRet
		
	}
	return null
}

ContainTType := !(Object^ toCheck) -> bool
{
	res := Queue.{string}()
	return ContainTType(toCheck,res)
}
ContainTType := !(Object^ toCheck,Queue.{string} res) -> bool
{
	if toCheck == null return false
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
			if Counter == 1 return true
			Counter = 0
		} else Counter += 1

		if iter is ObjData and iter.Down?.Right?.GetValue() == "..."
		{
			return true
		}

		if ContainTType(iter) return true //TODO: check after Syntax, not Before
	}
	if  Counter == 1 return true

	if sk.Right?.GetValue() == "." and sk.Right.Right?.GetValue() == "{}"
	{
		for iter : sk.Right.Right.Down
		{
			if iter.GetValue() != ","
			{
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
	this := !(Object^ inPars, Object^ inOutType, Object^ cons, bool RetRef, string SomeName, Object^ Stuf,bool IsSuf, Type^ metC, bool IsVirt,bool isRetSelf) -> void
	{
		IsSelfReturn = isRetSelf
		IsRetRef = RetRef
		IsVirtual = IsVirt
		FuncName = SomeName
		MethodType = metC
		NewFuncs."this"()

		
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
		firstNon := Object^
		firstNon = null
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
			asTC := metC->{TypeClass^}
			asTC.ToClass.ItTemplates.Push(this&)
		}
	}
	GetPriority :=virtual !(FuncInputBox itBox) -> int
	{
		parsCount := itBox.itPars.Size()
		FType := MyFuncType

		if itBox.itConsts.Size() != this.ItConsts.Size() return 255

		st := Queue.{ObjConstHolder^}()
		if not CheckTypes(itBox,st,null) {
			return 255
		}
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
			newRet = ParseType(CopyRet)
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

				if it.ptrToFunc.IsSameConsts(itBox) {
					return it.ptrToFunc
				} 
			}
			somePos += 1
		}
		newFunc := GetNewFunc(itBox,newFuncType)
		assert(newFunc != null)
		if IsSelfReturn
			newFunc.IsSelfReturn = true
		if IsPassAttrs
			newFunc.IsPassAttrs = true

		for  parConsts
		{
			newFunc.ItVals.Push(it)
		}

		for itBox.itConsts
			newFunc.ItConsts.Push(it)
		for value,key : itBox.itAttrs 	{ newFunc.ItAttrs[key] = value}
		
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

		return newFunc	
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
		iterS := Down
		while iterS != null
		{
			iterS.PrintGlobal(f)
			iterS = iterS.Right
		}
	}

	DoTheWork := virtual !(int pri) -> void
	{
	}
	GetValue := virtual !() -> string
	{
		return "!(){}"
	}
}


BoxFunc := class extend Object
{
	MyFuncType := TypeFunc^
	MyFuncParamNames := string[]
	FuncName := string
	OutputName := string
	ABox := AllocBox
	IsSuffix := bool
	IsVirtual := bool
	IsMethod := bool
	IsRetComplex := bool
	IsRetRef := bool
	IsStatic := bool
	IsInvalid := bool
	IsSelfReturn := bool
	IsPassAttrs := bool

	CountAttrs := bool

	VirtualId := int

	MethodType := Type^

	ItConsts := Queue.{Object^}
	ItAttrs := AVLMap.{string,Object^}
	ItVals := Queue.{ObjConstHolder^}

	vargsName := string
	funcUserParamsCount := int

	vargTypeBarrier := Object^

	GetType := virtual !() -> Type^
	{
		return MyFuncType
	}
	IsAssembler := virtual !() -> bool
	{
		return false
	}
	GetItem := virtual !(string name) -> Object^
	{
		iter := ItVals.Start

		while iter != null
		{
			if iter.Data.ItName == name  
			{
				return iter.Data.Down
			}
			iter = iter.Next
		}
		return null
	}
	
	IsSameConsts := !(FuncInputBox itBox) -> bool
	{
		if itBox.itConsts.Size() != this.ItConsts.Size() return false

		if CountAttrs
		{
			if itBox.itAttrs.Size() < this.ItAttrs.Size() return false
			for val,key : itBox.itAttrs
			{
				sRes :=  ItAttrs.TryFind(key)
				if sRes != null
				{
					if not CmpConstObjs(val,sRes^)
						return false
				}else{
					if val is ObjBool
					{
						asBool := val->{ObjBool^}
						if asBool.MyBool
							return false
					}else{
						return false
					}
				}
			}
		}

		for ct : itBox.itConsts , i : 0, tc : this.ItConsts
		{
			if not CmpConstObjs(ct,tc) 
			{
				return false
			}
		}
		return true
	}

	ItConstsTT := List.{Object^}
	ParseConsts := !(Object^ cons) -> void
	{
		if cons != null
		{
			iter := cons.Down
			while iter != null
			{
				if iter.GetValue() != ","
				{
					if iter.IsConst
					{
						this.ItConsts.Push(iter.Clone())
						ItConstsTT.Push(null)
					}else{
						typ := ParseType(iter)
						if typ != null
						{
							this.ItConsts.Push(new ObjType(typ))
							ItConstsTT.Push(null)
						}else{
							stdL := Queue.{string}()
							if ContainTType(iter,stdL)
							{
								this.ItConsts.Push(null->{Object^})
								ItConstsTT.Push(iter)
							}else{
								ErrorLog.Push("can not parse object in .{}\n")
							}
						}
					}
				}
				iter = iter.Right
			}
		}
	}

	ParseParams := !(Object^ root, Object^ outObj,bool IsTempl) -> bool
	{
		SyntaxCompress(root,PriorityData)
		Pars := Queue.{Object^}() ; $temp

		Typs := Queue.{Type^}() ; $temp
		TypsNams := Queue.{string}() ; $temp
		TypsIsRef := Queue.{bool}() ; $temp
		IsVargsL := false
		ContainTT := false

		RetTyp := null->{Type^}
		if GetValue() == "!()" RetTyp =  ParseType(outObj)
		if IsSelfReturn
		{
			RetTyp = MethodType
			IsRetRef = true
		}

		Stuff := Queue.{Object^}() ; $temp

		if RetTyp != null
		{
			if RetTyp is TypeArr or RetTyp is TypeClass
			{
				if not IsRetRef this.IsRetComplex = true
			}
		}
		if FuncName == "new" IsStatic = true

		if MethodType != null and (not IsStatic)
		{
			Typs.Push(MethodType)
			TypsNams.Push("this")
			TypsIsRef.Push(true)
		}

		Stuff.Push(root.Down[^])


		if Stuff.Size() != 0 Stuff.Push(new ObjSymbol(",")) ; $temp 

		IsParRef := false
		indType := 0
		while Stuff.NotEmpty()
		{
			if Stuff[0].GetValue() == ","
			{
				if Pars.Size() == 2
				{
					if Pars[0].Down?.Right.GetValue() == "..."
					{
						vargTypeBarrier = Pars[0].Down.Right
						if not Pars[1] is ObjIndent {
							EmitError("it should be indent")
							return false
						}
						vargsName = Pars[1]->{ObjIndent^}.MyStr
					}else{
						MayType := ParseType(Pars[0])
						indType += 1
						MayName := ""

						if MayType == null and not ContainTType(Pars[0])
						{
							if ContainTType(Pars[0]) or ContainTT
							{
								ContainTT = true
							}else
							{
								if not IsTempl
								{
									printf("can not parse type at %i\n",indType)
									return false
								}
							}
						}

						if Pars[1] is ObjIndent
						{
							MayName = (Pars[1]->{ObjIndent^}).MyStr
						}else{
							printf("only indentificators allowed, got %s\n",Pars[1].GetValue())
							return false
						}
						Typs.Push(MayType)
						TypsIsRef.Push(IsParRef) // TODO
						TypsNams.Push(StrCopy(MayName))
					}	
					Pars.Clean()		
				}
				if Pars.Size() == 1
				{
					if Pars[0].GetValue() == "..." IsVargsL = true
					else
					{
						if Pars[0].GetValue() == "~ind"
						{
							Typs.Push(null->{Type^})
							TypsIsRef.Push(IsParRef)
							TypsNams.Push((Pars[0]->{ObjIndent^}).MyStr)
						}else{
							itr := Pars[0]
							if itr is ObjData and itr.Down != null and itr.Down is ObjIndent
							and itr.Down.Right != null and itr.Down.Right.GetValue() == "..."
							{
								asIn := itr.Down->{ObjIndent^}
								vargsName = asIn.MyStr
							}else{
								EmitError("unknown error\n")
							}

						}
					}
					Pars.Clean()
				}
				Stuff.Pop()
			}else
			{
				Pars.Push(Stuff.Pop())
			}
		}
		MyFuncType = null
		MyFuncParamNames = null

		if Typs.Size() != 0
		{
			MyFuncParamNames = TypsNams.ToArray()
		}
		

		arr := TypsIsRef.ToArray()
		MyFuncType = GetFuncType(Typs,arr,RetTyp,IsRetRef,IsVargsL)
		funcUserParamsCount = MyFuncType.ParsCount

		if MyFuncParamNames != null and MyFuncType.ParsCount != 0
		{
			if MyFuncParamNames[0] == "this" IsMethod = true
		}

		if Typs.Size() == 0 return true

		return true
	}
	SetReturnType := !(Type^ toSet) -> void
	{
		this.IsRetComplex = false
		if toSet is TypeArr  or toSet is TypeClass
		{
			if not this.IsRetRef this.IsRetComplex = true
		}
		if MyFuncType.RetType != toSet
		{
			MyFuncType = ExchangeFuncType(MyFuncType,toSet)
		}			
	}
	ParseBlock := virtual !() -> void
	{
	}

	GetValue := virtual !() -> string
	{
		return "!()"
	}
	ApplyDeferUse := virtual !(int depth) -> void
	{
	}
}

BoxFuncDeclare := class  extend BoxFunc
{
	this := !(Object^ inPars, Object^ inOutType, string SomeName) -> void
	{
		IsRetRef = false
		FuncName = SomeName
		OutputName = StrCopy(SomeName)
		IsInvalid = not ParseParams(inPars,inOutType,false)

		if IsInvalid inOutType.EmitError("can not parse function\n")
		MethodType = null
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
		f << "declare "
		if OutCC != null{
			f << " " << OutCC << " "
		}
		MyFuncType.RetType.PrintType(f)
		f << " @" << OutputName
		MyFuncType.PrintSkobs(f)
		f << "\n"
	}

	DoTheWork := virtual !(int pri) -> void
	{
		//empty is ok
	}
}

PrintFuncBodySkobs := !(sfile f,TypeFunc^ fType,string[] names,string fName,string Extra,int itId) -> void
{
	f << "define " //"dso_local "

	if InCC != null
		f << " " << InCC << " "

	IsRetComplex := false

	if not fType.RetRef
	{
		IsRetComplex = IsComplexType(fType.RetType)
	}

	if IsRetComplex f << "void"
	else 
	{
		fType.RetType.PrintType(f)
		if fType.RetRef f << "*"
	}

	f << " @" << fName

	f << "("
	
	if Extra != null
	{
		f <<	Extra
		if IsRetComplex or fType.ParsCount != 0 
			f << " , "
	}
	if IsRetComplex 
	{
		f << fType.RetType.GetName() << "* %" << ReturnName
		if fType.ParsCount != 0 f << " , "
	}
	for i : fType.ParsCount
	{
		if i > 0 f << " , "
		if names != null and i < names->len
		{
			if fType.ParsIsRef[i]
				fType.Pars[i].GetPoint().PrintType(f)
			else	fType.Pars[i].PrintType(f)
			f << " %" <<names[i]
		}else{
			if fType.ParsIsRef[i]
				fType.Pars[i].GetPoint().PrintType(f)
			else	fType.Pars[i].PrintType(f)
			f << " %Extra" << itId << "Param" << i
		}
	}
	if fType.IsVArgs
	{
		if fType.ParsCount > 0 f << " , "
		f << "..."
	}
	f << ")"
}

BoxFuncBody := class extend BoxFunc
{
	parsed := bool
	ItParams := LocalParam^[]
	InAlloc := int[]
	ExtraRetParam := FuncParam^
	Parent := BoxFuncBody^
	
	GetScope := virtual !() -> int { return ABox.ItId }
	ApplyParams := !(int count,string^ names, Type^^ pars,bool^ isRef) -> void
	{
		if count != 0
		{
			ItParams = new LocalParam^[count]
			InAlloc = new int[count]

			for i : count
			{
				if isRef[i]{
					InAlloc[i] = ABox.GetAlloc(pars[i].GetPoint())
				}else{
					InAlloc[i] = ABox.GetAlloc(pars[i])
				}
				ItParams[i] = new LocalParam(pars[i],InAlloc[i],isRef[i])
			}
		}
	}
	
	this := !(string^ names, TypeFunc^ fType,string SomeName, Object^ Stuf,bool IsSuf,Type^ metC,bool IsVirt) -> void
	{
		ABox.ItId = GetNewId()
		IsRetRef = fType.RetRef
		MyFuncParamNames = names
		FuncName = SomeName
		MethodType = metC
		IsVirtual = IsVirt

		if SomeName == "main"
		{
			OutputName = "main"
		}else
		{
			OutputName = "func" + GetNewId()
		}

		if SomeName == "new" IsStatic = true

		MyFuncType = fType

		TestRet(fType.RetType)

		if MyFuncType != null 
		{
			ApplyParams(MyFuncType.ParsCount,names,MyFuncType.Pars,MyFuncType.ParsIsRef)
		}

		if Stuf.GetValue() == "{}"
		{
			Down = new BoxBlock()
			Down.Line = Line
			Down.Up = this&
			if Stuf.Down != null
			{
				Down.Down = Stuf.Down
				Stuf.Down.SetUp(Down)
			}

		}else{
			ErrorLog.Push("CompilerError: function with weird body\n")
		}
		if IsInvalid EmitError("can not parse function header\n")

		IsSuffix = IsSuf
		if MyFuncParamNames != null
		{
				
		}

		if metC != null{
			
			asCls := metC->{TypeClass^}
			asClsT := asCls.ToClass
			asClsT.ItMethods.Push(this&->{BoxFunc^})

			if IsVirtual
			{
				ParseBlock()
				asClsT.PutVirtualFunc(FuncName MyFuncType this&->{BoxFunc^})
			}
		}
	}
	TestRet := !(Type^ t) -> void
	{
		if t != null
		{
			this.IsRetComplex = false
			if t is TypeArr or t is TypeClass
			{
				if not IsRetRef
				{
					this.IsRetComplex = true
					if ExtraRetParam == null
						ExtraRetParam = new FuncParam(ReturnName,t,true)
				}
			}
		}
	}
	this := !(Object^ inPars, Object^ inOutType,Object^ cons,bool RetRef, string SomeName, Object^ Stuf,bool IsSuf,Type^ metC,bool IsVirt,bool isRetSelf) -> void
	{
		IsSelfReturn = isRetSelf
		ABox.ItId = GetNewId()
		IsRetRef = RetRef
		IsVirtual = IsVirt
		FuncName = SomeName
		MethodType = metC
		if SomeName == "main"
		{
			OutputName = "main"
		}else
		{
			OutputName = "func" + GetNewId()
		}
		if SomeName == "new" {
			IsStatic = true
		}

		if SomeName == "~this"
		{
			if metC == null
			{
				EmitError("~this outside class\n")
			}else{
				asC := metC->{TypeClass^}
				toC := asC.ToClass
				if toC.ContainVirtual{
					IsVirtual = true
				}
			}
		}
		if inOutType != null inOutType.Up = this&

		IsInvalid = not ParseParams(inPars,inOutType,false)
		ParseConsts(cons)

		if MyFuncType != null TestRet(MyFuncType.RetType)

		if MyFuncType != null 
		{
			ApplyParams(MyFuncType.ParsCount,MyFuncParamNames,MyFuncType.Pars,MyFuncType.ParsIsRef)
		}

		if Stuf.GetValue() == "{}"
		{
			Down = new BoxBlock()
			Down.Up = this&
			Down.Line = Line
			Down.Down = Stuf.Down
			Down->{BoxBlock^}.usePaths = true
			if Stuf.Down != null Stuf.Down.SetUp(Down)

		}else{
			ErrorLog.Push("CompilerError: function with weird body\n")
		}
		if IsInvalid inPars.EmitError("can not parse function header\n" + "func name " + FuncName + "\n")

		IsSuffix = IsSuf
		for c : FuncName
		{
			if not (c in 'a'..'z') and not (c in 'A'..'Z') IsSuffix = false 
		}

		if metC != null {
			
			asCls := metC->{TypeClass^}
			asClsT := asCls.ToClass
			asClsT.ItMethods.Push(this&->{BoxFunc^})

			if IsVirtual 
			{
				ParseBlock()
				asNeed := (metC->{TypeClass^}).ToClass
				asNeed.PutVirtualFunc(FuncName,MyFuncType,this&)
			}
		}
		if FuncName == "main"
		{
			ParseBlock()
		}
	}
	AddFuncCall := !(Object^ itm) -> void
	{
		itr := Down.Down

		if itr == null
		{
			Down.Down = itm
			itm.Up = Down
		}else{
			while itr.Right != null
			{
				itr = itr.Right
			}
			itr.Right = itm
			itm.Left = itr
			itm.Up = itr
		}
	}
	ParseBlock := virtual !() -> void
	{
		if not parsed 
		{
			if FuncName == "~this" and MethodType != null
			{
				pL := new FuncParam("this",MethodType,true)
				if this.MethodType is TypeClass
				{
					asCT := MethodType->{TypeClass^}
					asC := asCT.ToClass

					if asC.Parent != null
					{
						box5 := new FuncInputBox() 
						stp := asC.Parent
						box5.itPars.Emplace(stp.ClassType,true)
						func2 := asC.Parent.GetFunc("~this",box5^,true)

						if func2 != null
						{
							pCall := new ParamNaturalCall("",pL->{Object^})
							fCall := MakeSimpleCall(func2,pCall)
							AddFuncCall(fCall)
						}else{
							//EmitError("compiler error 82346\n")
						}
					}
					WorkBag.Push(this&,State_PrePrint)
				}
				
			}
			
			parsed = true
			WorkBag.Push(Down,State_Start)
			Down.Up = this&
		}
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
		ABox.PrintGlobal(f)

		if MyFuncType.RetType != null and parsed
		{
			PrintGlobalSub(f)
			PrintFuncBodySkobs(f,MyFuncType,MyFuncParamNames,OutputName,null->{string},ABox.ItId)

			f << " #0 "

			if DebugMode
			{
				f << " !dbg !" << ABox.ItId
			}

			f << " ; " << FuncName

			f << "\n{\n"

			ABox.PrintAlloc(f)
			//f << "%ABoxSizePre" << ABox.ItId << " = getelementptr %AllocClass" << ABox.ItId << " , %AllocClass" << ABox.ItId << "* null , i32 1\n"
			//f << "%ABoxSize" << ABox.ItId << " = ptrtoint %AllocClass" << ABox.ItId << "* %ABoxSizePre" << ABox.ItId <<" to i32\n"
			//f << "%ABoxPoint = bitcast %AllocClass" << ABox.ItId << "* %AllocItem" << ABox.ItId << " to i8*\n" 
			//f << "call void @memset(i8* %ABoxPoint" << ABox.ItId << " , i8 0, i32 %ABoxSize" << ABox.ItId << ")\n"
			if DebugMode PrintDebugDeclare(f,null)

			if InAlloc != null
			for i : MyFuncType.ParsCount
			{
					f << "store "
					f << MyFuncType.Pars[i].GetName()
					if MyFuncType.ParsIsRef[i] f << "*"
				if MyFuncParamNames != null and i < MyFuncParamNames->len
				{
					f << " %" << MyFuncParamNames[i] << " , "
				}else{
					f << " %Extra" << ABox.ItId << "Param" << i << " , "
				}
					f << MyFuncType.Pars[i].GetName()
					if MyFuncType.ParsIsRef[i] f << "*"
					f << "* %T" << InAlloc[i] << "\n"
				if DebugMode and MyFuncParamNames != null and i < MyFuncParamNames->len
				{
					if MyFuncType.ParsIsRef[i]
					{
						outId := CreateDbgLocVar(this&,MyFuncType.Pars[i],MyFuncParamNames[i],true)
						newId := CreateDebugCall(this&)
						if newId != -1 and outId != -1
						{
							f << "call void @llvm.dbg.declare(metadata " << MyFuncType.Pars[i].GetName() << "** %T" << InAlloc[i] << " , metadata !" << outId << " , metadata !DIExpression()) , !dbg !" << newId << "\n"
						}
					}else{
						outId := CreateDbgLocVar(this&,MyFuncType.Pars[i],MyFuncParamNames[i])
						newId := CreateDebugCall(this&)
						if newId != -1 and outId != -1
						{
							f << "call void @llvm.dbg.declare(metadata " << MyFuncType.Pars[i].GetName() << "* %T" << InAlloc[i] << " , metadata !" << outId << " , metadata !DIExpression()) , !dbg !" << newId << "\n"
						}
					}
				}
			}
			
			iterP := Up

			while iterP != null
			{
				if iterP.GetValue() == "x=>x"
				{
					asL := iterP->{SLambda^}
					ABName := asL.ABox.GetClassName()
					f << "%ItHiddenName" << ABox.ItId << " = bitcast i8* %HiddenName to "  <<ABName << "*\n"
					asL.ABox.PrintBoxItems(f,"%ItHiddenName" + ABox.ItId)

					if not asL.justFunc
					{
						f << "%ItBdPre" << ABox.ItId << " = getelementptr " << ABName << " , " << ABName << "* %ItHiddenName"<<ABox.ItId<<", i32 0,i32 " 
							<< asL.ABox.GetNR(asL.InAlloc[0]) << "\n"
						f << "%ItBd  = load i8*, i8** " << "%ItBdPre" << ABox.ItId << "\n"
						asL.PrintInhers(f,"ItBd")
					}
					break
				}
				if iterP.GetValue() == "!()" and iterP != this&
				{
					asN := iterP->{BoxFuncBody^}
					ABName := asN.ABox.GetClassName()
					if not asN.ABox.ItemBag.Empty()
					{
						f << "%ItHiddenName" << ABox.ItId << " = bitcast i8* %HiddenName to " << ABName << "*\n"
						asN.ABox.PrintBoxItems(f,"%ItHiddenName" + ABox.ItId)
						if asN.IsMethod
						{
							thisId := asN.ItParams[0].inAllocId
							fT := asN.MyFuncType
							f << "%thisPre = getelementptr " << fT.Pars[0].GetName() << "* , " << fT.Pars[0].GetName() << "** %T" << thisId << " , i32 0\n"
							f << "%this = load " << fT.Pars[0].GetName() << "* , " << fT.Pars[0].GetName() << "** %thisPre\n" 
						}else{
							//printf("nope\n")
						}
					}
					break
				}
				iterP = iterP.Up
			}

			if (not this.IsRetComplex) and (MyFuncType.RetType != GetType("void")) and (MyFuncType.RetType.GetName() != "void")
			{
				RT := MyFuncType.RetType
				f << "%Result = alloca " << RT.GetName()
				if MyFuncType.RetRef f << "*"
				f << "\n"
				if MyFuncType.RetRef
				{
					f << "store " << RT.GetName() << "* null , " << RT.GetName() << "** %Result\n"
				}else{
					if IsBool(RT)
					{
						f << "store i1 0,i1* %Result\n"
					}else
					if IsInt(RT)
					{
						f << "store " << RT.GetName() << " 0 , " << RT.GetName() << "* %Result\n"
					}else
					if IsFloat(RT)
					{
						f << "store " << RT.GetName() << " 0.0 , " << RT.GetName() << "* %Result\n"
					}else
					if RT is TypePoint or RT is TypeFatArr
					{
						f << "store " << RT.GetName() << " null , " << RT.GetName() << "* %Result\n"
					}
				}
			}

			iter := Down
			while iter != null
			{
				iter.PrintInBlock(f)
				iter = iter.Right
			}
			f << "br label %OutLabel" << ABox.ItId << "\n"
			f << "OutLabel" << ABox.ItId << ":\n"

			if MyFuncType.RetType == GTypeVoid or this.IsRetComplex
			{
				f << "ret void\n"
			}else{
				retTypeName := MyFuncType.RetType.GetName()
				if MyFuncType.RetRef retTypeName = retTypeName + "*"
				f << "%ResultItem = load " << retTypeName << " , " << retTypeName << "* %Result\n"
				f << "ret " << retTypeName << " " << "%ResultItem\n"
			}

			f << "}\n"

			if DebugMode
			{	
				iter := Up
				if iter != null
				{
					while iter.Up != null
					{
						iter = iter.Up
					}
				}
				if iter != null
				{
					asN := iter->{BoxFile^}
					f << "!" << ABox.ItId << " = distinct !DISubprogram(name:\"" << FuncName << "\","
					f << "scope: !" << asN.fileId << " , file: !" << asN.fileId
					f << ",type: !" << MyFuncType.metaId 
					if Line != null
					{
						f << ", line: " << Line.LinePos	
						f << ", scopeLine: " << Line.LinePos	
					}
					f << ", unit: !" << cuId
					f << ", flags: DIFlagPrototyped"
					f << ")\n"

				}
			}
		}
	}
	GetOutPath := virtual !(Object^ item, int typ,int size) ->string
	{
		return "OutLabel" + ABox.ItId
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			WorkBag.Push(this&,State_Syntax)
		}

		if pri == State_Syntax
		{
			//SyntaxCompress(this&,PriorityData)
			//UnboxParams(this.Down)
			WorkBag.Push(this&,State_ErrorCheck)
		}
		if pri == State_ErrorCheck
		{
			if MyFuncType == null
			{
				ErrorLog.Push("wut?\n")
			}else{
				if MyFuncType.RetType == null
					SetReturnType(GetType("void"))
			}
		}
		if pri == State_PrePrint
		{
			asCT := MethodType->{TypeClass^}
			asC := asCT.ToClass
					
			i := asC.NotMineParams
			EndS := asC.Params.Size()
			for nowField : asC.Params, i : 0
			{
				sBug := nowField->{Object^}
				itCPre := sBug.GetType()
				if itCPre is TypeClass
				{
					itC := itCPre->{TypeClass^}.ToClass

					box3 := new FuncInputBox()

					box3.itPars.Emplace(itCPre,true)
					func3 := itC.GetFunc("~this",box3^,true)
					if func3 != null
					{
						box4 := new FuncInputBox() 
						box4.itConsts.Push(new ObjStr(nowField.ItName))
						fnc := asC.AutoFieldTemplate
						pr := fnc.GetPriority(box4^)
						if pr == 0
						{
							asT := fnc->{BoxTemplate^}
							funcCl := asT.GetFunc(box4^)
							funcCll := MakeSimpleCall(funcCl,null->{Object^})

							exF := MakeSimpleCall(func3,funcCll)
							AddFuncCall(exF)
						}
						//exF := MakeSimpleCall(func3,pCall2)
						//AddFuncCall(exF)
					}else{
						EmitError("compiler error 2345\n")
					}
				}
			}
		}
	}
}

GetFuncBlock := !(Object^ to) -> BoxFunc^
{
	iterE := to

	while iterE != null
	{
		if iterE.GetValue() == "!()"
		{
			return iterE->{BoxFunc^}
		}
		iterE = iterE.Up
	}
	return null
}
