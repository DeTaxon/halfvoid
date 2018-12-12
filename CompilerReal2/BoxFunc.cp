#import "Tree.cp"
#import "StandartObjects.cp"
#import "AllocBox.cp"
#import "ConstTemp.cp"
#import "FindFunc.cp"
#import "FuncCalls.cp"
#import "FuncInputBox.cp"

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

	constsI := null->{Object^}


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
		if IsTemplate(ParamsObj)
		{
			return new BoxTemplate(ParamsObj,RetT,constsI,RetRef,FName,iter,IsSuf,ClassType,IsVirtual)
		}
		PreRet :=  new BoxFuncBody(ParamsObj,RetT,constsI,RetRef,FName,iter,IsSuf,ClassType,IsVirtual)
		
		return PreRet
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
	bag := Stack.{Object^}()
	bag.Push(toCheck)

	while bag.NotEmpty()
	{
		item := bag.Pop()
		if item.GetValue() == "~{}type" 
		{
			asNeed := item->{ObjTemplateType^}
			res.Push(asNeed.MyStr)
			added = true
			//return true
		}

		iter := item.Down

		while iter != null
		{
			bag.Push(iter)
			iter = iter.Right
		}
	}
	return added
}

IsTemplate := !(Object^ sk) -> bool
{
	if sk == null return false
	iter := sk.Down

	Counter := 0

	while iter != null
	{
		if iter.GetValue() == ","
		{
			if Counter == 1 return true
			Counter = 0
		} else Counter += 1
		if ContainTType(iter) return true //TODO: check after Syntax, not Before
		iter = iter.Right
	}
	if  Counter == 1 return true

	lazy := sk.Right != null
	if lazy lazy = sk.Right.GetValue() == "."
	if lazy lazy = sk.Right.Right != null
	if lazy lazy =  sk.Right.Right.GetValue() == "{}"
	if lazy
	{
		iter = sk.Right.Right.Down

		while iter != null
		{
			if iter.GetValue() != ","
			{
				if not iter.IsConst() 
					if ParseType(iter) == null
						return true
				if ContainTType(iter) return true
			}
			iter = iter.Right
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

BoxTemplate := class extend BoxFunc
{
	CopyParams := Object^
	CopyConsts := Object^
	CopyRet := Object^
	CopyTree := Object^

	EndPos := Object^
	FuncsType := Queue.{TypeFunc^}
	//FuncsConsts := Queue.{Queue.{Object^}}

	FuncsTTemps := Queue.{Object^}
	TTNames := Queue.{string}

	TempReturnConsts := Queue.{ObjConstHolder^}^

	IsVirtual := bool

	CheckTypes := !(FuncInputBox itBox,Queue.{ObjConstHolder^} res) -> bool
	{
		re := true
		for fT : FuncsTTemps, i: 0, par : itBox.itPars
		{
			if MyFuncType.Pars[i] == null
			{
				if fT != null
				{
					IsSameType(fT,par.first,res,re&)
					if not re return false
				}
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
			iter := TempReturnConsts.Start

			while iter != null
			{
				if iter.Data.ItName == name
				{
					return iter.Data.Down
				}
				iter = iter.Next
			}
		}
		return null
	}
	this := !(Object^ inPars, Object^ inOutType, Object^ cons, bool RetRef, string SomeName, Object^ Stuf,bool IsSuf, Type^ metC, bool IsVirt) -> void
	{
		IsRetRef = RetRef
		IsVirtual = IsVirt
		FuncName = SomeName
		MethodType = metC
		
		if inPars != null {
			CopyParams = inPars.Clone()
			CopyParams.SetUp(this&)
		}
		if cons != null
		{
			CopyConsts = cons.Clone()
			ParseConsts(CopyConsts)
			CopyConsts.Up = this&
			MakeGoodConsts(CopyConsts)
		}
		if inOutType != null {
			CopyRet = inOutType.Clone()
			CopyRet.SetUp(this&)
		}
		if Stuf != null CopyTree = Stuf.Clone()

		IsSuffix = IsSuf
		ParseParams(CopyParams,CopyRet,true)

		SyntaxCompress(CopyParams.Down,PriorityData)
		iter := CopyParams.Down
		firstNon := Object^
		firstNon = null
		//FuncsTs := Queue.{Object^}() 


		while iter != null
		{
			if iter.GetValue() == ","
			{
				if ContainTType(firstNon,TTNames)
				{
					FuncsTTemps.Push(firstNon)
				}else{
					FuncsTTemps.Push(null->{Object^})
				}
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
				if ContainTType(firstNon,TTNames)
				{
					FuncsTTemps.Push(firstNon)
				}else{
					FuncsTTemps.Push(null->{Object^})
				}
				firstNon = null
			}else{
				if firstNon == null 
				{
					firstNon = iter
				}
			}
		}
		if not IsWord(SomeName) IsSuffix = false
	}
	GetPriority :=virtual !(FuncInputBox itBox) -> int
	{
		parsCount := itBox.itPars.Size()
		FType := MyFuncType

		if itBox.itConsts.Size() != this.ItConsts.Size() return 255

		st := Queue.{ObjConstHolder^}()
		if not CheckTypes(itBox,st) return 255

		if parsCount == FType.ParsCount or (FType.IsVArgs and parsCount >= FType.ParsCount)
		{
			IsCorrect := true

			MaxPrior := 0

			for i : FType.ParsCount, par : itBox.itPars
			{
				SomePrior := 0
				if FType.Pars[i] != null and FuncsTTemps[i] == null
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
		outT := Queue.{Type^}()
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
		newRet := MyFuncType.RetType
		if MyFuncType.RetType == null and CopyRet != null
		{
			newRet = ParseType(CopyRet)
		}
		return GetFuncType(outT,MyFuncType.ParsIsRef,newRet,MyFuncType.RetRef,MyFuncType.IsVArgs)
	}
	GetFunc := virtual !(FuncInputBox itBox) -> BoxFunc^
	{
		outT := Queue.{Type^}()

		parConsts := Queue.{ObjConstHolder^}()
		CheckTypes(itBox,parConsts)

		TempReturnConsts = parConsts& //TODO: replace
		newFuncType := CreateFuncPointer(itBox)
		TempReturnConsts = null

		iterJ := FuncsType.Start
		somePos := 0
		while iterJ != null
		{
			if iterJ.Data == newFuncType 
			{
				inDown := Down
				for somePos inDown = inDown.Right
				asNeed :=  inDown->{BoxFunc^}

				if asNeed.IsSameConsts(itBox) {
					return asNeed
				} 
			}
			iterJ = iterJ.Next
			somePos += 1
		}
		newFunc := GetNewFunc(itBox,newFuncType)

		for  parConsts
		{
			newFunc.ItVals.Push(it)
		}
		
		if newFunc == null return null

		FuncsType.Push(newFuncType)

		WorkBag.Push(newFunc,State_Start)

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
		for itBox.itConsts newFunc.ItConsts.Push(it)
		for value,key : itBox.itAttrs 	{ newFunc.ItAttrs[key] = value}
		newFunc.ParseBlock()

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
	MyFuncParamNames := string^
	FuncName := string
	OutputName := string
	ABox := AllocBox
	IsSuffix := bool
	IsVirtual := bool
	IsMethod := bool
	IsRetComplex := bool
	IsRetRef := bool
	IsStatic := bool

	VirtualId := int

	MethodType := Type^

	ItConsts := Queue.{Object^}
	ItAttrs := AVLMap.{string,Object^}
	ItVals := Queue.{ObjConstHolder^}

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
		if itBox.itAttrs.Size() != this.ItAttrs.Size() return false

		for ct : itBox.itConsts , i : 0, tc : this.ItConsts
		{
			if not CmpConstObjs(ct,tc) 
			{
				return false
			}
		}
		return true
	}
	ParseConsts := !(Object^ cons) -> void
	{
		if cons != null
		{
			iter := cons.Down
			while iter != null
			{
				if iter.GetValue() != ","
				{
					if iter.IsConst()
					{
						this.ItConsts.Push(iter.Clone())
					}else{
						typ := ParseType(iter)
						if typ != null
						{
							this.ItConsts.Push(new ObjType(typ))
						}else{
							stdL := Queue.{string}()
							if ContainTType(iter,stdL)
							{
								this.ItConsts.Push(null->{Object^})
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
		iter := root.Down
		Pars := Queue.{Object^}()

		Typs := Queue.{Type^}()
		TypsNams := Queue.{string}()
		TypsIsRef := Queue.{bool}()
		IsVargsL := false
		ContainTT := false

		RetTyp := null->{Type^}
		//if GetValue() == "!()" RetTyp =  ParseType(outObj)
		RetTyp = ParseType(outObj)

		Stuff := Queue.{Object^}()

		if RetTyp != null
		{
			if RetTyp.GetType() == "arr" or RetTyp.GetType() == "class"
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


		while iter != null
		{
			Stuff.Push(iter)
			iter = iter.Right
		}

		if Stuff.Size() != 0 Stuff.Push(new ObjSymbol(","))

		IsParRef := false
		indType := 0
		while Stuff.NotEmpty()
		{
			if Stuff[0].GetValue() == ","
			{
				if Pars.Size() == 2
				{
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

					if Pars[1].GetValue() == "~ind" 
					{
						MayName = (Pars[1]->{ObjIndent^}).MyStr
					}else{
						printf("only indentificators allowed\n")
						return false
					}
					Typs.Push(MayType)
					TypsIsRef.Push(IsParRef) // TODO
					TypsNams.Push(StrCopy(MayName))
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
		if toSet.GetType() == "arr" or toSet.GetType() == "class"
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
}

BoxFuncDeclare := class  extend BoxFunc
{
	this := !(Object^ inPars, Object^ inOutType, string SomeName) -> void
	{
		IsRetRef = false
		FuncName = SomeName
		OutputName = StrCopy(SomeName)
		IsInvalid = not ParseParams(inPars,inOutType,false)

		if IsInvalid ErrorLog.Push("can not parse function\n")
		MethodType = null
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
		f << "declare "
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

PrintFuncBodySkobs := !(sfile f,TypeFunc^ fType,string^ names,string fName,string Extra) -> void
{
	f << "define "

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
		f << fType.RetType.GetName() << "* %ToRet"
		if fType.ParsCount != 0 f << " , "
	}
	for i : fType.ParsCount
	{
		
		if i > 0 f << " , "
		if fType.ParsIsRef[i]
			fType.Pars[i].GetPoint().PrintType(f)
		else	fType.Pars[i].PrintType(f)
		f << " %" <<names[i]
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
	ItParams := LocalParam^^
	InAlloc := int^	
	ExtraRetParam := FuncParam^
	Parent := BoxFuncBody^

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
			if t.GetType() == "arr" or t.GetType() == "class"
			{
				if not IsRetRef
				{
					this.IsRetComplex = true
					if ExtraRetParam == null
						ExtraRetParam = new FuncParam("ToRet",t,true)
				}
			}
		}
	}
	this := !(Object^ inPars, Object^ inOutType,Object^ cons,bool RetRef, string SomeName, Object^ Stuf,bool IsSuf,Type^ metC,bool IsVirt) -> void
	{
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
			printf("creating static\n")
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
		//inOutType.Up = this&

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
			Down.Down = Stuf.Down
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
				if this.MethodType.GetType() == "class"
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
			PrintFuncBodySkobs(f,MyFuncType,MyFuncParamNames,OutputName,null->{string})

			if DebugMode
			{
				f << " !dbg !" << ABox.ItId
			}

			f << " ; " << FuncName

			f << "\n{\n"

			ABox.PrintAlloc(f)

			if InAlloc != null
			for i : MyFuncType.ParsCount
			{
				f << "store "
				f << MyFuncType.Pars[i].GetName()
				if MyFuncType.ParsIsRef[i] f << "*"
				f << " %" << MyFuncParamNames[i] << " , "
				f << MyFuncType.Pars[i].GetName()
				if MyFuncType.ParsIsRef[i] f << "*"
				f << "* %T" << InAlloc[i] << "\n"
			}
			
			iterP := Parent

			while iterP != null
			{
				asN := iterP
				ABName := asN.ABox.GetClassName()
				//f << "%LBegin" << nameIter << "Pos = getelementptr " << ABName << " , " << ABName<< "* null ,i32 0, i32 " << ItNR << "\n"
				//f << "%LS2" << nameIter << " = ptrtoint " << ResultType.GetName() << " %LBegin" << nameIter << "Pos to i64\n"
				//f << "%LS1" << nameIter << " = ptrtoint i8* %"<< prevLName  << " to i64\n"
				//f << "%Lambda" << nameIter << "Pre2 = sub i64 %LS1" << nameIter << " , %LS2" << nameIter << "\n"
				//f << "%Lambda" << nameIter << "Box = inttoptr i64 %Lambda" << nameIter << "Pre2 to " << ABName << "*\n"
				f << "%ItHiddenName" << ABox.ItId << " = bitcast i8* %HiddenName to " << ABName << "*\n"
				asN.ABox.PrintBoxItems(f,"%ItHiddenName" + ABox.ItId)
				if asN.IsMethod
				{
					fT := asN.MyFuncType
					//f << "%thisPre = getelementptr " << ABName << " , " << ABName << "* %ItHiddenName" + ABox.ItId + " , i32 0,i32 0\n"
					//f << "%this = load " << fT.Pars[0].GetName() << "* , " << fT.Pars[0].GetName() << "** %thisPre\n" 
					f << "%this = getelementptr " << ABName << " , " << ABName << "* %ItHiddenName" + ABox.ItId + " , i32 0,i32 0\n"
				}else{
					//printf("nope\n")
				}
				iterP = iterP.Parent
			}

			if (not this.IsRetComplex) and (MyFuncType.RetType != GetType("void"))
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
					}
					if IsInt(RT)
					{
						f << "store " << RT.GetName() << " 0 , " << RT.GetName() << "* %Result\n"
					}
					if IsFloat(RT)
					{
						f << "store " << RT.GetName() << " 0.0 , " << RT.GetName() << "* %Result\n"
					}
					if RT.GetType() == "point" or RT.GetType() == "fatarr"
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

			if MyFuncType.RetType == GetType("void") or this.IsRetComplex
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
					}
					f << ", unit: !" << cuId
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
				if itCPre.GetType() == "class"
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
