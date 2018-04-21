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
	ParamsObj := iter
	iter = iter.Right

	if iter.GetValue() == "->"
	{
		iter = iter.Right
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
			return new BoxTemplate(ParamsObj,RetT,FName,iter,IsSuf,ClassType,IsVirtual)
		}
		PreRet :=  new BoxFuncBody(ParamsObj,RetT,FName,iter,IsSuf,ClassType,IsVirtual)
		
		return PreRet
	}
	return null
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
		iter = iter.Right
	}
	return false
}

BoxTemplate := class extend BoxFunc
{
	CopyParams := Object^
	CopyRet := Object^
	CopyTree := Object^

	EndPos := Object^
	FuncsType := Queue.{TypeFunc^}
	FuncsConsts := Queue.{Queue.{Object^}}

	IsVirtual := bool


	this := !(Object^ inPars, Object^ inOutType, string SomeName, Object^ Stuf,bool IsSuf, Type^ metC, bool IsVirt) -> void
	{
		IsVirtual = IsVirt
		FuncName = SomeName
		MethodType = metC
		
		if inPars != null CopyParams = inPars.Clone()
		if inOutType != null CopyRet = inOutType.Clone()
		if Stuf != null CopyTree = Stuf.Clone()

		IsSuffix = IsSuf
		ParseParams(CopyParams,CopyRet)
	}
	GetValue := virtual !() -> string
	{
		return "!(){}"
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
			return MaxPrior
		}
		return 255	
	}
	GetFunc := virtual !(Queue.{Type^} pars) -> BoxFunc^
	{
		zeto := Queue.{Object^}()
		return GetFunc(pars,zeto)
	}
	GetFunc := virtual !(Queue.{Type^} pars,Queue.{Object^} consts) -> BoxFunc^
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

		newFuncType := GetFuncType(outT,MyFuncType.ParsIsRef,MyFuncType.RetType,MyFuncType.RetRef,MyFuncType.IsVArgs)

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
}


BoxFunc := class extend Object
{
	MyFuncType := TypeFunc^
	MyFuncParamNames := string^
	FuncName := string
	OutputName := string
	ABox := AllocBox
	IsSuffix := bool

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

		Stuff := Queue.{Object^}()

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

					if MayType == null
					{
						printf("can not parse type\n")
						return false
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
		
		RetTyp := ParseType(outObj)

		arr := TypsIsRef.ToArray()
		MyFuncType = GetFuncType(Typs,arr,RetTyp,false,IsVargsL)
		free(arr)

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
	IsVirtual := bool
	this := !(string^ names, TypeFunc^ fType,string SomeName, Object^ Stuf,bool IsSuf,Type^ metC,bool IsVirt) -> void
	{
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
	}
	this := !(Object^ inPars, Object^ inOutType, string SomeName, Object^ Stuf,bool IsSuf,Type^ metC,bool IsVirt) -> void
	{
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
			asNeed := (metC->{TypeClass^}).ToClass
			asNeed.PutVirtualFunc(FuncName,MyFuncType,this&)
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
		if MyFuncType.RetType != null
		{
			PrintGlobalSub(f)
			f << "define "
			MyFuncType.RetType.PrintType(f)
			f << " @" << OutputName

			f << "("
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

			f << "\n{\n"

			ABox.PrintAlloc(f)

			iter := Down
			while iter != null
			{
				iter.PrintInBlock(f)
				iter = iter.Right
			}

			if MyFuncType.RetType == GetType("void")
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
