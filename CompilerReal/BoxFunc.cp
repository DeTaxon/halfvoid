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
	
	if iter.GetValue() == "declare"
	{
		return new BoxFuncDeclare(ParamsObj,RetT,FName)
	}
	if iter.GetValue() == "{}"
	{
		if IsTemplate(ParamsObj)
		{
			return new BoxTemplate(ParamsObj,RetT,RetRef,FName,iter,IsSuf,ClassType,IsVirtual)
		}
		PreRet :=  new BoxFuncBody(ParamsObj,RetT,RetRef,FName,iter,IsSuf,ClassType,IsVirtual)
		
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
	return Counter == 1 
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
	CopyRet := Object^
	CopyTree := Object^

	EndPos := Object^
	FuncsType := Queue.{TypeFunc^}
	FuncsConsts := Queue.{Queue.{Object^}}

	FuncsTTemps := Queue.{Object^}
	TTNames := Queue.{string}

	IsVirtual := bool

	ComputeTypes := !(Queue.{Type^} pars,Queue.{Object^} res) -> bool
	{
		for TTNames.Size() res.Push(null->{Object^})
		MiniBag := Queue.{MiniWork}()
	
		for i : FuncsTTemps.Size()
		{
			if FuncsTTemps[i] != null
			{
				MiniBag.Push(MiniWork(FuncsTTemps[i],pars[i]))
			}
		}

		iter := MiniBag.Start
		NowWork := MiniWork

		while iter != null
		{
			NowWork = iter.Data
			NowNod := NowWork.SomeNode
			NowType := NowWork.SomeType
			NowVal := NowWork.SomeNode.GetValue()
			MiniBag.JustPopFront()
			iter = iter.Next

			if NowVal  == "~d"
			{
				if Down.Right == "[]"
				{
						
				}
			}else{
				if NowVal == "~ind"
				{
					asNeed := NowVal->{ObjIndent^}

					objType := Object^
					objType = null
					for TTNames.Size()
					{
						if asNeed.MyStr == TTNames[it] objType = res[it]
					}
					if objType != null
					{
						printf("here\n")
						minT := ObjType(NowType)
						return CmpConstObjs(objType,minT&->{Object^})
					}

					//if gotType == null 
				}else{
					if NowVal == "~{}type"
					{
						asNeed := NowNod->{ObjTemplateType^}
						for i : TTNames.Size()
						{	
							if TTNames[i] == asNeed.MyStr
							{
								res[i] = new ObjType(NowType)
								//iter = MiniBag.Start
							}
						}
					}
				}
			}
		}
		printf("wut %i\n",MiniBag.Size())
		return MiniBag.Empty()

		return false
	}
	this := !(Object^ inPars, Object^ inOutType, bool RetRef, string SomeName, Object^ Stuf,bool IsSuf, Type^ metC, bool IsVirt) -> void
	{
		IsRetRef = RetRef
		IsVirtual = IsVirt
		FuncName = SomeName
		MethodType = metC
		
		if inPars != null CopyParams = inPars.Clone()
		if inOutType != null CopyRet = inOutType.Clone()
		if Stuf != null CopyTree = Stuf.Clone()

		IsSuffix = IsSuf
		ParseParams(CopyParams,CopyRet)

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
	}
	GetPriority :=virtual !(Queue.{Type^} pars, Queue.{Object^} consts) -> int
	{
		parsCount := pars.Size()
		FType := MyFuncType
		if parsCount == FType.ParsCount or (FType.IsVArgs and parsCount >= FType.ParsCount)
		{
			IsCorrect := true
			iterT := pars.Start

			MaxPrior := 0

			for i : FType.ParsCount
			{
				SomePrior := 0
				if FType.ParsIsRef[i] 
				{
					if iterT.Data != FType.Pars[i] SomePrior = 255
				}else {
					SomePrior = TypeCmp(iterT.Data, FType.Pars[i])
				}
				if MaxPrior < SomePrior MaxPrior = SomePrior
				iterT = iterT.Next
			}
			temp := Queue.{Object^}()
			if ComputeTypes(pars,temp) return MaxPrior
			return 255 //MaxPrior
		}
		return 255	
	}
	GetFunc := virtual !(Queue.{Type^} pars) -> BoxFunc^
	{
		zeto := Queue.{Object^}()
		return GetFunc(pars,zeto)
	}
	CreateFuncPointer := virtual !(Queue.{Type^} pars, Queue.{Object^} consts) -> TypeFunc^
	{
		outT := Queue.{Type^}()
		FType := MyFuncType
		
		for FType.ParsCount
		{
			if FType.Pars[it] == null
			{
				outT.Push(pars[it])
			}else{
				outT.Push(FType.Pars[it])
			}
		}
		return GetFuncType(outT,MyFuncType.ParsIsRef,MyFuncType.RetType,MyFuncType.RetRef,MyFuncType.IsVArgs)
	}
	GetFunc := virtual !(Queue.{Type^} pars,Queue.{Object^} consts) -> BoxFunc^
	{
		outT := Queue.{Type^}()

		newFuncType := CreateFuncPointer(pars,consts)

		iterJ := FuncsType.Start
		somePos := 0
		while iterJ != null
		{
			if iterJ.Data == newFuncType 
			{
				PointQ := FuncsConsts[somePos]&
				Found := true

				if consts.Size() == PointQ^.Size()
				{
					for consts.Size()
					{
						if not CmpConstObjs(consts[it],PointQ^[it])
							Found = false
					}
				}else{
					Found = false
				}
				if Found
				{
					inDown := Down
					for somePos inDown = inDown.Right
					return inDown->{BoxFunc^}
				}
			}
			iterJ = iterJ.Next
			somePos += 1
		}

		newFunc := GetNewFunc(pars,consts,newFuncType)
		
		if newFunc == null return null

		FuncsType.Push(newFuncType)
		FuncsConsts.Push(consts)

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
	GetNewFunc := virtual !(Queue.{Type^} pars,Queue.{Object^} consts, TypeFunc^ FunType) -> BoxFunc^
	{

		newFunc := new BoxFuncBody(MyFuncParamNames,FunType,FuncName,CopyTree.Clone(),IsSuffix,MethodType,IsVirtual)
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

	MethodType := Type^

	GetType := virtual !() -> Type^
	{
		return MyFuncType
	}
	IsAssembler := virtual !() -> bool
	{
		return false
	}
	ParseParams := !(Object^ root, Object^ outObj) -> bool
	{
		SyntaxCompress(root,PriorityData)
		iter := root.Down
		Pars := Queue.{Object^}()

		Typs := Queue.{Type^}()
		TypsNams := Queue.{string}()
		TypsIsRef := Queue.{bool}()
		IsVargsL := false
		ContainTT := false

		RetTyp := ParseType(outObj)

		Stuff := Queue.{Object^}()

		if RetTyp != null
		{
			if RetTyp.GetType() == "arr" or RetTyp.GetType() == "class"
			{
				IsRetComplex = true
			}
		}

		if MethodType != null
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
		while Stuff.NotEmpty()
		{
			if Stuff[0].GetValue() == ","
			{
				if Pars.Size() == 2
				{
					MayType := ParseType(Pars[0])
					MayName := ""

					if MayType == null and not ContainTType(Pars[0])
					{
						if ContainTType(Pars[0]) or ContainTT
						{
							ContainTT = true
						}else
						{
							printf("can not parse type\n")
							return false
						}
					}

					if Pars[1].GetValue() == "~ind" 
					{
						MayName = (Pars[1]->{ObjIndent^}).MyStr
					}else{
						printf("only indentificator allowed\n")
						return false
					}
					Typs.Push(MayType)
					TypsIsRef.Push(IsParRef) // TODO
					TypsNams.Push(MayName.Copy())
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
		free(arr)

		if MyFuncParamNames != null and MyFuncType.ParsCount != 0
		{
			if MyFuncParamNames[0] == "this" IsMethod = true
		}

		if Typs.Size() == 0 return true

		return true
	}
	SetReturnType := !(Type^ toSet) -> void
	{
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
		OutputName = SomeName.Copy()
		IsInvalid = not ParseParams(inPars,inOutType)

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

BoxFuncBody := class extend BoxFunc
{
	parsed := bool
	ItParams := FuncParam^^
	ExtraRetParam := FuncParam^

	//TODO UserConsts := Queue.{Object^} // !().{T}
	HiddenConsts := Queue.{ObjConstHolder^} // !(@T x)


	this := !(string^ names, TypeFunc^ fType,string SomeName, Object^ Stuf,bool IsSuf,Type^ metC,bool IsVirt) -> void
	{
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
		MyFuncType = fType

		TestRet(fType.RetType)


		if MyFuncType != null 
		{
			if MyFuncType.ParsCount != 0
			{
				ItParams = new FuncParam[MyFuncType.ParsCount]

				for MyFuncType.ParsCount
				{
					ItParams[it] = new FuncParam(names[it],MyFuncType.Pars[it],MyFuncType.ParsIsRef[it])
				}
			}
		}

		if Stuf.GetValue() == "{}"
		{
			Down = new BoxBlock()
			Down.Up = this&
			Down.Down = Stuf.Down
			Stuf.Down.SetUp(Down)

		}else{
			ErrorLog.Push("CompilerError: function with weird body\n")
		}
		if IsInvalid ErrorLog.Push("can not parse function header\n")

		IsSuffix = IsSuf
		if MyFuncParamNames != null
		{
				
		}
		if IsVirt and metC != null
		{
			ParseBlock()
			asNeed := (metC->{TypeClass^}).ToClass
			asNeed.PutVirtualFunc(FuncName,MyFuncType,this&)
		}
	}
	TestRet := !(Type^ t) -> void
	{
		if t != null
		{
			if t.GetType() == "arr" or t.GetType() == "class"
			{
				if ExtraRetParam == null
					ExtraRetParam = new FuncParam("ToRet",t,true)
			}
		}
	}
	this := !(Object^ inPars, Object^ inOutType,bool RetRef, string SomeName, Object^ Stuf,bool IsSuf,Type^ metC,bool IsVirt) -> void
	{
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
		IsInvalid = not ParseParams(inPars,inOutType)

		if MyFuncType != null TestRet(MyFuncType.RetType)

		if MyFuncType != null 
		{
			if MyFuncType.ParsCount != 0
			{
				ItParams = new FuncParam[MyFuncType.ParsCount]

				for MyFuncType.ParsCount
				{
					ItParams[it] = new FuncParam(MyFuncParamNames[it],MyFuncType.Pars[it],MyFuncType.ParsIsRef[it])
				}
			}
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
		if IsInvalid ErrorLog.Push("can not parse function header\n")

		IsSuffix = IsSuf

		if IsVirt and metC != null
		{
			ParseBlock()
			asNeed := (metC->{TypeClass^}).ToClass
			asNeed.PutVirtualFunc(FuncName,MyFuncType,this&)
		}
		if FuncName == "main"
		{
			ParseBlock()
		}
	}
	ParseBlock := virtual !() -> void
	{
		if not parsed
		{
			parsed = true
			WorkBag.Push(Down,State_Start)
		}
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
		if MyFuncType.RetType != null and parsed
		{
			PrintGlobalSub(f)
			f << "define "

			if IsRetComplex f << "void"
			else 
			{
				MyFuncType.RetType.PrintType(f)
				if IsRetRef f << "*"
			}

			f << " @" << OutputName

			f << "("
			if IsRetComplex 
			{
				f << MyFuncType.RetType.GetName() << "* %ToRet"
				if MyFuncType.ParsCount != 0 f << " , "
			}
			for i : MyFuncType.ParsCount
			{
				
				if i > 0 f << " , "
				if MyFuncType.ParsIsRef[i]
					MyFuncType.Pars[i].GetPoint().PrintType(f)
				else	MyFuncType.Pars[i].PrintType(f)
				f << " %" <<MyFuncParamNames[i]
			}
			if MyFuncType.IsVArgs
			{
				if MyFuncType.ParsCount > 0 f << " , "
				f << "..."
			}
			f << ")"
			f << " ; " << FuncName

			f << "\n{\n"

			ABox.PrintAlloc(f)

			iter := Down
			while iter != null
			{
				iter.PrintInBlock(f)
				iter = iter.Right
			}

			if MyFuncType.RetType == GetType("void") or IsRetComplex
				f << "ret void\n"

			f << "}\n"
		}
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			WorkBag.Push(this&,State_Syntax)
		}

		if pri == State_Syntax
		{
			SyntaxCompress(this&,PriorityData)
			UnboxParams(this.Down)
			WorkBag.Push(this&,State_GetUse)
		}
		if pri == State_GetUse
		{
			iter := Down
			
			if iter != null
			{
				while iter.Right != null iter = iter.Right
				while iter != null
				{
					WorkBag.Push(iter,State_Start)
					iter = iter.Left
				}
			}
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
