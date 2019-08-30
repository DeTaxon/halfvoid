SLambda := class extend ObjResult
{
	parsedStart := bool
	applyed := bool
	ABox := AllocBox
	Names := string[]
	parms := LocalParam^^
	InAlloc := int^
	fastUse := TypeFunc^
	ItId := int
	ItNR := int
	inAlloc := int
	manSkob := bool
	justFunc := bool

	StolenNames := Queue.{string}
	StolenParams := Queue.{MemParam^}

	this := !() -> void
	{
		ABox.ItId = GetNewId()
		WorkBag.Push(this&,State_Start)
		ItId = GetNewId()
		inAlloc = -1
	}
	ApplyParams := !(int count, Type^^ pars,bool^ isRef) -> void
	{
		if count != 0
		{
			parms = new LocalParam^[count]
			InAlloc = new int[count]
		}
		for i : count
		{
			if isRef[i] {
				InAlloc[i] = ABox.GetAlloc(pars[i].GetPoint())
			}else{
				InAlloc[i] = ABox.GetAlloc(pars[i])
			}
			parms[i] = new LocalParam(pars[i],InAlloc[i],isRef[i])
		}

		WorkBag.Push(Down,State_Syntax)

		if justFunc
		{
			ResultType = ResultType.Base.GetPoint()
		}


	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start and not parsedStart
		{
			parsedStart = true
			justFunc = Down.Right.GetValue() == "=>"
			//WorkBag.Push(this&,State_Syntax)
			names := Queue.{string}() ; $temp

			if not justFunc
				names.Push("lambdaParam" + ItId)

			pars := Queue.{Type^}()

			skobPos := Object^
			skobPos = null
			if Down.GetValue() == "()"
			{
				skobPos = Down
			}else{
				if Down.GetValue() == "~d"
				{
					if Down.Down.GetValue() == "()"
					{
						skobPos = Down.Down					
					}
				}
			}

			if skobPos != null
			{
				TrimCommas(skobPos)
				iter := skobPos.Down
				while iter != null
				{
					if iter.GetValue() == "~ind"
					{
						asN := iter->{ObjIndent^}
						names.Push(asN.MyStr)
						pars.Push(null->{Type^})
					}else{
						if iter.GetValue() != ","
							EmitError("incorrect input of lambda\n")
					}
					iter = iter.Right
				}
			}else{
				if Down.GetValue() == "~ind"
				{
					asN := Down->{ObjIndent^}
					names.Push(asN.MyStr)
					pars.Push(null->{Type^})
				}else{
					EmitError("incorrect input items of lambda\n")
				}
			}
			Names = names.ToArray()

			asFunc := GetFuncType(pars,null->{bool^},null->{Type^},false,false)


			ResultType = asFunc.GetLambda()
			PopOutNode(Down)
			PopOutNode(Down)

			if Down.GetValue() == "{}"
				manSkob = true
			MakeItBlock(Down)
		}
		if pri == State_PostGetUse
		{
			inAlloc = GetAlloc(Up,ResultType.Base.GetPoint())
			ItNR = GetAllocNR(Up,inAlloc)
		}
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
		ABox.PrintGlobal(f)
		Down.PrintGlobal(f)
		
		if applyed
		{
			PrintFuncBodySkobs(f,fastUse,Names,"lambda" + ItId,null->{string},ABox.ItId)

			f << "\n{\n"
			ABox.PrintAlloc(f)
			for i : fastUse.ParsCount
			{
				f << "store "
				f << fastUse.Pars[i].GetName()
				if fastUse.ParsIsRef[i] f << "*"
				f << " %" << Names[i] << " , "
				f << fastUse.Pars[i].GetName()
				if fastUse.ParsIsRef[i] f << "*"
				f << "* %T" << InAlloc[i] << "\n"
			}
			
			nameIter := 0
			prevLName := Names[0]
			
			if not justFunc
			{
				iter := Up
				while iter != null
				{
					if iter.GetValue() == "!()"
					{
						asN := iter->{BoxFuncBody^}
						ABName := asN.ABox.GetClassName()
						f << "%LBegin" << nameIter << "Pos = getelementptr " << ABName << " , " << ABName<< "* null ,i32 0, i32 " << ItNR << "\n"
						f << "%LS2" << nameIter << " = ptrtoint " << ResultType.GetName() << " %LBegin" << nameIter << "Pos to i64\n"
						f << "%LS1" << nameIter << " = ptrtoint i8* %"<< prevLName  << " to i64\n"
						f << "%Lambda" << nameIter << "Pre2 = sub i64 %LS1" << nameIter << " , %LS2" << nameIter << "\n"
						f << "%Lambda" << nameIter << "Box = inttoptr i64 %Lambda" << nameIter << "Pre2 to " << ABName << "*\n"
						asN.ABox.PrintBoxItems(f,"%Lambda" + nameIter + "Box")
						if asN.IsMethod
						{
							fT := asN.MyFuncType
							//f << "%thisPre = getelementptr " << ABName << " , " << ABName << "* %Lambda" << nameIter << "Box , i32 0,i32 0\n"
							//f << "%this = load " << fT.Pars[0].GetName() << "* , " << fT.Pars[0].GetName() << "** %thisPre\n" 
							f << "%this = getelementptr " << ABName << " , " << ABName << "* %Lambda" << nameIter << "Box , i32 0,i32 0\n"
						}else{
							//printf("nope\n")
						}
									
					}
					iter = iter.Up
				}
			}
			IsRetComplex := false
			if not fastUse.RetRef
			{
				IsRetComplex = IsComplexType(fastUse.RetType)
			}
			if (not IsRetComplex) and fastUse.RetType != GTypeVoid
			{
				f << "%Result = alloca " << fastUse.RetType.GetName()
				if fastUse.RetRef f << "*"
				f << "\n"
			}

			Down.PrintInBlock(f)
			f << "br label %OutLabel" << ABox.ItId << "\n"
			f << "OutLabel" << ABox.ItId << ":\n"

			if fastUse.RetType == GTypeVoid or IsRetComplex
			{
				f << "ret void\n"
			}else{
				retTypeName := fastUse.RetType.GetName()
				if fastUse.RetRef retTypeName = retTypeName + "*"
				f << "%ResultItem = load " << retTypeName << " , " << retTypeName << "* %Result\n"
				f << "ret " << retTypeName << " " << "%ResultItem\n"
			}
			f << "}\n"
		}

	}
	PrintPre := virtual !(sfile f) -> void
	{
		if justFunc return void
		if applyed
		{
			asL := ResultType->{TypeFuncLambda^}
			f << "store " + asL.GetPointName() + " @lambda" + ItId + ", " + ResultType.GetName() + " %T" + inAlloc + "\n"
		}
	}
	PrintUse := virtual !(sfile f) -> void
	{	
		if justFunc
		{
			ResultType.PrintType(f)
			f << " @lambda" << ItId
		}else{
			ResultType.PrintType(f)
			f << " %T" << inAlloc
		}
	}
	GetName := virtual !() -> string
	{
		if justFunc
		{
			return "@lambda" + ItId
		}
		return "%T" + inAlloc
	}
	ApplyFunc := !(Type^ lambTyp, bool isFnc) -> void
	{
		if isFnc xor justFunc
		{
			if isFnc
			{
				EmitError("can not change lambda to func")
			}else{
				EmitError("can not change func to lambda")
			}
			return void
		}
		asFType := ((lambTyp.Base)->{TypeFunc^})

		if not manSkob
		{
			isRetCm := IsComplexType(asFType.RetType)

			if asFType.RetType != GTypeVoid and not isRetCm
			{
				UNext(Down.Down,new ObjObj("return()"),1)
			}
		}
		fastUse = asFType
		applyed = true
		ResultType = lambTyp
		WorkBag.Push(Down,State_Start)
		WorkBag.Push(this&,State_PostGetUse)

		ApplyParams(fastUse.ParsCount,fastUse.Pars,fastUse.ParsIsRef)
	}
	GetItem := virtual !(string name) -> Object^
	{
		for i : fastUse.ParsCount 
		{
			if Names[i] == name
			{
				return parms[i]
			}
		}

		if StolenNames[^i] == name
			return StolenParams[i]

		return null
	}

	GetOutPath := virtual !(Object^ item, int typ,int size) ->string
	{
		return "OutLabel" + ABox.ItId
	}
	GetValue := virtual !() -> string
	{
		return "x=>x"
	}
	ApplyDeferUse := virtual !(int depth) -> void
	{
		printf("sfgasfhafg\n")
	}
}
