ParseFuncDataR := !(Object^ item) -> Object^
{
	iter := item.Down

	if iter == null return null

	IsStatic := false
	IsVirtual := false

	ClassPtr := BoxClass^()
	ClassType := Type^()

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


	ClassPtr := GetUpClass(item)
	if(ClassPtr != null)
	{
		ClassType = ClassPtr.ClassType
	}

	ExtraIter := item
	while ExtraIter != null
	{
		switch ExtraIter.GetValue()
		{
		case "~fake"
			if IsVirtual
			{
				item.EmitError("can not use virtual functions inside fake field")
				return null
			}
			ExtraIter = ExtraIter.Up
		case void
			ExtraIter = ExtraIter.Up
		}
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
	isPoison := false
	FName := ""
	IsSuf := false

	while iterForName != null
	{
		if iterForName.GetValue() == "i:=1"
		{
			dynCa := iterForName->{ObjParam^}
			IsSuf = dynCa.IsStrName
			FName = dynCa.MyStr
			isPoison = dynCa.IsPoison

			if FName == "![]" IsSuf = false
			if FName == "this" IsSuf = false

			if IsOper(FName) IsSuf = false

			break
		}else	iterForName = iterForName.Up
	}
	if constsI != null	SyntaxCompress(constsI,PriorityData)
	if iter.GetValue() == "declare"
	{
		return new BoxFuncDeclare(ParamsObj,RetT,FName)
	}

	if isPoison
	{
		asCl := GetUpClass(iterForName)
		if asCl != null
		{
			asCl.ItPoisons[FName] = item.Clone()
		}
	}
	if iter.GetValue() == "{}"
	{
		if constsI?.Down == null or IsTemplate(ParamsObj)
		{
			preRes := new BoxTemplate(ParamsObj,RetT,constsI,RetRef,FName,iter,IsSuf,ClassType,IsVirtual,itsSelfRet,item)
			preRes.IsSelfReturn = itsSelfRet
			preRes.Line = itLine
			return preRes
		}
		preRet := new BoxFuncBody(ParamsObj,RetT,constsI,RetRef,FName,iter,IsSuf,ClassType,IsVirtual,itsSelfRet,item)
		preRet.IsSelfReturn = itsSelfRet
		preRet.Line = itLine
		return preRet
		
	}
	return null
}


IsBoxFuncContainer := !(Object^ toCheck) -> bool
{
	if toCheck is BoxFuncBody return true
	if IsSLambda(toCheck) return true
	if toCheck is BoxFuncBodyFromString return true
	return false
}

BoxFuncContainer := class extend Object
{
	ABox := AllocBox
	yodlerInAlloc := int
	Yodlers := List.{BoxReturn^}

	GetABox := virtual !() -> AllocBox^ {return ABox&}
	AddYodler := virtual !(BoxReturn^ toAdd) -> int
	{
		return 0
	}

	containDefer := bool
	DoDefer := !() -> void
	{
		if not containDefer
			WorkBag.Push(this&,State_CheckDefer)
		containDefer = true
	}
	GetFuncBody := virtual !() -> BoxFuncContainer^
	{
		return this&
	}
	PrintABoxData := virtual !(TIOStream f, char^ objName,int debId) -> void
	{
		ABox.PrintBoxItems(f,objName,debId)
		if this& is BoxFuncBody
		{
			bdy := this&->{BoxFuncBody^}
			if bdy.IsMethod
			{
				thisId := bdy.ItParams[0].inAllocId
				fT := bdy.MyFuncType
				f << "%thisPre = getelementptr " << fT.Pars[0].GetName() << "* , " << fT.Pars[0].GetName() << "** %T" << thisId << " , i32 0\n"
				f << "%this = load " << fT.Pars[0].GetName() << "* , " << fT.Pars[0].GetName() << "** %thisPre\n" 
			}
		}
	}

}

BoxFunc := class extend BoxFuncContainer
{
	MyFuncType := TypeFunc^
	MyFuncParamNames := string[]
	FuncName := string
	OutputName := string
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
	ItVals := List.{ObjConstHolder^}

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
	GenerateCall := virtual !(Object^ pars) -> BaseFuncCall^
	{
		return null
	}
	GetItem := virtual !(string name) -> Object^
	{
		if ItVals[^].ItName == name
			return it.Down
		return null
	}
	
	IsSameConsts := !(FuncInputBox itBox) -> bool
	{
		if itBox.itConsts.Size() != ItConsts.Size() 
			return false

		if CountAttrs
		{
			if itBox.itAttrs.Size() < ItAttrs.Size() 
				return false
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

		for ct,i : itBox.itConsts , tc : ItConsts
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
			for iter : cons.Down
			{
				if iter.GetValue() == ","
					continue

				if iter.IsConst
				{
					ItConsts.Push(iter.Clone())
					ItConstsTT.Push(null)
					continue
				}
				typ := ParseType(iter)
				if typ != null
				{
					ItConsts.Push(new ObjType(typ))
					ItConstsTT.Push(null)
					continue
				}

				stdL := Queue.{string}()
				if ContainTType(iter,stdL)
				{
					ItConsts.Push(null->{Object^})
					ItConstsTT.Push(iter)
				}else{
					ErrorLog.Push("can not parse object in .{}\n")
				}
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

		Stuff.Push(root.Down[^]) ; $temp


		if Stuff.Size() != 0 Stuff.Push(new ObjSymbol(",")) ; $temp 

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
						IsParRef := false
						MayType := Type^()
						if Pars[0].Down?.Right?.GetValue() == "&"
						{
							MayType = ParseType(Pars[0].Down)
							IsParRef = true
						}else{
							MayType = ParseType(Pars[0])
						}
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
						TypsIsRef.Push(IsParRef)
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
							TypsIsRef.Push(false)
							TypsNams.Push((Pars[0]->{ObjIndent^}).MyStr)
						}else{
							itr := Pars[0]
							if itr is ObjData and itr.Down? is ObjIndent and itr.Down.Right?.GetValue() == "..."
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
		OutputName = StrCopy(SomeName)
		FuncName = OutputName
		IsInvalid = not ParseParams(inPars,inOutType,false)

		if IsInvalid inOutType.EmitError("can not parse function\n")
		MethodType = null
	}
	this := !(TypeFunc^ fT, string SomeName) -> void
	{
		FuncName = SomeName
		OutputName = SomeName
		MyFuncType = fT
	}
	PrintGlobal := virtual !(TIOStream f) -> void
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

PrintFuncBodySkobs := !(TIOStream f,TypeFunc^ fType,string[] names,string fName,string Extra,int itId) -> void
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

BoxFuncBodyFromString := class extend BoxFuncBody
{
	this := !(char^ funcName ,Type^ fType,char^ strToParse) -> void
	{
		ABox.ItId = GetNewId()
		outLabel."this"()
		MyFuncType = fType->{TypeFunc^}
		OutputName = funcName
		FuncName = OutputName
		GetObjectsFromMemory(Path(""),strToParse,strlen(strToParse),this&)
		if Down != null
		{
			itr := Down
			while itr.Right != null itr = itr.Right
			UNext(Down, new ObjSkobs("{}"),itr)
			MakeItBlock(Down)
		}
		WorkBag.Push(Down,State_Start)
		WorkBag.Push(this&,State_Start)
		parsed = true
	}
	GetScope := virtual !() -> int { return -1 }
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
