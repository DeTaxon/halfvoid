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

	Yodlers := List.{BoxReturn^}
	YodlerState := MemParam^

	StolenParams := AVLMap.{char^,LocalParam^}

	tupleItem := TupleClass^

	CaptureParams := List.{Tuple.{char^,MemParam^,bool,MemParam^}}
	CPIndexes := List.{int}

	this := !() -> void
	{
		ABox.ItId = GetNewId()
		WorkBag.Push(this&,State_Start)
		ItId = GetNewId()
		inAlloc = -1
	}
	Clone := virtual !() -> Object^
	{
		PreRet := new SLambda()
		PreRet.Line = Line

		IterD := Down
		LineI := Object^
		LineI = null
		
		while IterD != null
		{
			if LineI == null
			{
				PreRet.Down = IterD.Clone()
				LineI = PreRet.Down
			}else{
				LineI.Right = IterD.Clone()
				LineI.Right.Left = LineI
				LineI = LineI.Right
			}
			IterD = IterD.Right
		}
		if PreRet.Down != null PreRet.Down.SetUp(PreRet)

		return PreRet
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

			asFunc := GetFuncType(pars,null->{bool^},GTypeVoid,false,false)
			fastUse = asFunc

			if justFunc
			{
				ResultType = asFunc.GetPoint()
			}else{
				ResultType = asFunc.GetLambda()
			}
			PopOutNode(Down)
			PopOutNode(Down)

			if Down.GetValue() == "[]"
			{
				count := 0
				for c : Down.Down
				{
					if c.GetValue() != ","
					{
						if count == 0
						{
							if c.GetValue() == "~ind"
							{
								CaptureParams.Emplace(c->{ObjIndent^}.MyStr,null,false,null->{MemParam^})
								count = 1
							}else
							{
								EmitError("invalid capture input")
							}
						}else{
							if count == 1
							{	
								if c.GetValue() == "&"
								{
									CaptureParams.Back().2 = true
								}else
								{
									EmitError("unknown object")
								}
							}else{
								EmitError("too much stuf")
							}
						}
					}else{
						count = 0
					}
				}
				PopOutNode(Down)
			}

			if Down.GetValue() == "{}"
				manSkob = true
			MakeItBlock(Down)
		}
		if pri == State_PostGetUse
		{
			datRes := new FuncInputBox ; $temp
			datRes.itPars.Emplace(ResultType.Base.GetPoint(),false)
			datRes.itPars.Emplace(copyLFuncTypeP,false)
			datRes.itPars.Emplace(deleteLFuncTypeP,false)
			
			if Yodlers.Size() != null
				datRes.itPars.Emplace(GTypeInt,false)

			tupleItem = GetTuple(datRes)
			inAlloc = GetAlloc(Up,tupleItem.ClassType)
			ItNR = GetAllocNR(Up,inAlloc)
		}
		if pri == State_PrePrint
		{
			ApplyCaptures()
		}
	}
	PrintInhers := !(sfile f,char^ startS) -> void
	{
		funcsUp := List.{Tuple.{AllocBox^,SLambda^,BoxFuncBody^}}() ; $temp	
		itrr := Up
		while itrr != null
		{
			if itrr.GetValue() == "!()"
			{
				funcsUp.Emplace(itrr->{BoxFuncBody^}.ABox&,null,itrr->{BoxFuncBody^})
				break
			}
			if itrr is SLambda
			{
				asSL := itrr->{SLambda^}
				funcsUp.Emplace(asSL.ABox&,asSL,null)
			}
			itrr = itrr.Up
		}
		nameIter := 0
		prevLam := this&
		prevLName := startS
		for it : funcsUp
		{
			ABName := it.0.GetClassName()
			f << "%LBegin" << nameIter << "Pos = getelementptr " << ABName << " , " << ABName<< "* null ,i32 0, i32 " << prevLam.ItNR << ",i32 0\n"
			f << "%LS2" << nameIter << " = ptrtoint " << prevLam.ResultType.GetName() << " %LBegin" << nameIter << "Pos to i64\n"
			f << "%LS1" << nameIter << " = ptrtoint i8* %"<< prevLName  << " to i64\n"
			f << "%Lambda" << nameIter << "Pre2 = sub i64 %LS1" << nameIter << " , %LS2" << nameIter << "\n"
			f << "%Lambda" << nameIter << "Box = inttoptr i64 %Lambda" << nameIter << "Pre2 to " << ABName << "*\n"
			it.0.PrintBoxItems(f,"%Lambda"sbt + nameIter + "Box")
			prevLam = it.1
			if prevLam != null
			{
				f << "%NextLam" << nameIter << " = load i8* , i8** %T" << prevLam.InAlloc[0] << "\n"
				prevLName = "NextLam" + nameIter
			}
			if it.2 != null and it.2.IsMethod
			{
				itrT := it.2->{Object^}
				while itrT != null and itrT.GetValue() != "{...}"
					itrT = itrT.Up
				
				f << "%this = load " << itrT->{BoxClass^}.ClassType.GetName() << "* , "
					<< itrT->{BoxClass^}.ClassType.GetName() << "** %T" << it.2.InAlloc[0] <<"\n"
			}
			nameIter += 1
		}
		if applyedCaptures
		{
			k := 0
			for it : CaptureParams
			{
				if it.1 == null
					continue
				kk := CPIndexes[k]
				if it.2
				{
						f << "%TPr" << ABox.ItId << "l" << kk << " = getelementptr " << captureType.GetName() << " , "
						 	<< captureType.GetName() << "* %T" << cptInAlloc << ", i32 0, i32 " << kk << "\n"
						f << "%T" << ABox.ItId << "l" << kk << " = load "
						f << it.1.ResultType.GetName() << "* , " << it.1.ResultType.GetName() << "** %TPr" <<ABox.ItId << "l" << kk << "\n"
				}else{
						f << "%T" << ABox.ItId << "l" << kk << " = getelementptr " << captureType.GetName() << " , "
						 	<< captureType.GetName() << "* %T" << cptInAlloc << ", i32 0, i32 " << kk << "\n"
				}
				k++
			}
		}
	}
	
	applyedCaptures := bool
	GotCapture := bool
	captureType := Type^
	cptInAlloc := int
	cptInAllocNR := int
	ApplyCaptures := !() -> void
	{
		if applyedCaptures
			return void
		applyedCaptures = true

		iter := Up
		while iter != null
		{
			if iter is SLambda
				iter->{SLambda^}.ApplyCaptures()
			iter = iter.Up
		}

		toR := new FuncInputBox ; $temp	
		GotCapture = false 

		for it,i : CaptureParams
		{
			if it.1 != null
			{
				GotCapture = true
				CPIndexes.Push(i)
				if it.2
				{
					toR.itPars.Emplace(it.1.ResultType.GetPoint(),true)
				}else{
					toR.itPars.Emplace(it.1.ResultType,true)
				}
			}
		}
		if GotCapture
		{
			captureType = GetTuple(toR).ClassType
			cptInAlloc = GetAlloc(Up,captureType)
			cptInAllocNR = GetAllocNR(Up,cptInAlloc)
		}
	}



	PrintGlobal := virtual !(sfile f) -> void
	{
		if Yodlers.Size() != 0
		{
			if justFunc
			{
				ABox.liveOnGlobal = true
			}else{
				//TODO:
			}
		}
		Down.PrintGlobal(f)
		
		if applyed
		{
			ABox.PrintGlobal(f)
			
			funcsUp := List.{Tuple.{AllocBox^,SLambda^,BoxFuncBody^}}() ; $temp	
			nameIter := 0
			prevLName := Names[0]
			if not justFunc
			{
				itrr := Up
				while itrr != null
				{
					if itrr.GetValue() == "!()"
					{
						funcsUp.Emplace(itrr->{BoxFuncBody^}.ABox&,null,itrr->{BoxFuncBody^})
						break
					}
					if itrr is SLambda
					{
						asSL := itrr->{SLambda^}
						funcsUp.Emplace(asSL.ABox&,asSL,null)
					}
					itrr = itrr.Up
				}
				
				f << "%FatLambdaType" << ItId << " = type {"
				for bx,i : funcsUp
				{
					if i != 0
						f << ","
					f << bx.0.GetClassName()
				}
				f << "}\n"


				prevLambd := this&
				f << "define void @LambdaDelete" << ItId << "(i8* %ToDel" << ItId << ")\n"
				f << "{\n"
					ABName3 := funcsUp[0].0.GetClassName()
					f << "%LBegin" << nameIter << "Pos = getelementptr " << ABName3 << " , " << ABName3 << "* null ,i32 0, i32 " << prevLambd.ItNR << ",i32 0\n"
					f << "%LS2" << nameIter << " = ptrtoint " << ResultType.GetName() << " %LBegin" << nameIter << "Pos to i64\n"
					f << "%LS1" << nameIter << " = ptrtoint i8* %ToDel"<< ItId  << " to i64\n"
					f << "%Lambda" << nameIter << "Pre2 = sub i64 %LS1" << nameIter << " , %LS2" << nameIter << "\n"
					f << "%Lambda" << nameIter << "Box = inttoptr i64 %Lambda" << nameIter << "Pre2 to i8*\n"
					f << "call void @free(i8* %Lambda" << nameIter << "Box)\n"
				f << "ret void\n"
				f << "}"

				prevLName = "ToCpy" + ItId

				prevLambd = this&

				f << "define i8* @LambdaCopy" << ItId << "(i8* %ToCpy" << ItId << ")\n"
				f << "{"
				PrintInhers(f,"ToCpy" + ItId)
				
				ABName := ""
				for funcsUp
				{
					ABName = it.0.GetClassName()
					break
				}

				f << "%ResSizePre" << ItId << " = getelementptr %FatLambdaType" << ItId << " , %FatLambdaType"
					<< ItId << "* null , i32 1\n"
				f << "%ResSize" << ItId << " = ptrtoint %FatLambdaType" << ItId << "* %ResSizePre" << ItId << " to i64\n"
				f << "%Res" << ItId << " = call i8* @malloc(i64 %ResSize" << ItId << ")\n"
				f << "%PreSet" << ItId << " = bitcast i8* %Res" << ItId << " to " << ABName << "*\n"
				f << "%PreApply = bitcast i8* %Res" << ItId << " to %FatLambdaType" << ItId << "*\n"


				asL := ResultType->{TypeFuncLambda^}
				f << "%PreSetPoint" << ItId << " = getelementptr " << ABName << " , " << ABName << "* %PreSet" << ItId << " , i32 0, i32 " << ItNR << ", i32 0\n"
				f << "store " << asL.GetPointName() << " @lambda" << ItId << ", " << ResultType.GetName() << " %PreSetPoint" << ItId << "\n"

				f << "%PreSetCopy" << ItId << " = getelementptr " << ABName << " , " << ABName << "* %PreSet" << ItId << " , i32 0, i32 " << ItNR << ", i32 1\n"
				f << "store i8*(i8*)* @LambdaCopy"<< ItId <<", i8*(i8*)** %PreSetCopy" << ItId << "\n"

				f << "%PreSetDelete" << ItId << " = getelementptr " << ABName << " , " << ABName << "* %PreSet" << ItId << " , i32 0, i32 " << ItNR << ", i32 2\n"
				f << "store void(i8*)* @LambdaDelete"<< ItId <<", void(i8*)** %PreSetDelete" << ItId << "\n"

				if Yodlers.Size() != 0
				{
					upABox := funcsUp[0].0
					f << "%Yodler = getelementptr " << upABox.GetClassName() << " , " 
						<< upABox.GetClassName() << "* %Lambda0Box, i32 0, i32 " << ItNR << ",i32 3\n"
					f << "%PreSetYodler" << ItId << " = getelementptr " << ABName << " , " << ABName << "* %PreSet" << ItId << " , i32 0, i32 " << ItNR << ", i32 3\n"
					f << "%YodlerValue = load i32 , i32* %Yodler\n"
					f << "store i32 %YodlerValue , i32* %PreSetYodler" << ItId << "\n"
				}
				
				prevL := SLambda^()
				for fc : funcsUp, j : 0
				{
					if prevL != null
					{
						f << "%WherePut" << j << " = getelementptr %FatLambdaType"<< ItId <<", %FatLambdaType" << ItId << "* %PreApply , i32 0 , i32 " << j - 1
							<< ", i32 " << prevL.ABox.GetNR(prevL.InAlloc[0]) << "\n"
						if fc.1 != null
						{
							f << "%ToSetP" << j << " = getelementptr %FatLambdaType"<< ItId <<", %FatLambdaType" << ItId << "* %PreApply , i32 0, i32 " 
								<< j << ", i32 " << fc.1.ItNR << ",i32 0\n"
						}else{
							prevNR := fc.2.ABox.GetNR(prevL.inAlloc)
							f << "%ToSetP" << j << " = getelementptr %FatLambdaType"<< ItId <<", %FatLambdaType" << ItId << "* %PreApply , i32 0, i32 " 
								<< j << ", i32 " << prevNR << ", i32 0\n"
						}
						f << "%ToSetPT" << j << " = bitcast void(i8*)** %ToSetP" << j << " to i8*\n"
						f << "store i8* %ToSetPT" << j << " , i8** %WherePut" << j << "\n"
					}
					if fc.2 != null and fc.2.IsMethod
					{
						f << "%SetThis = getelementptr %FatLambdaType"<< ItId <<" , %FatLambdaType" << ItId
							<< "* %PreApply, i32 0, i32 " << j << ",i32 " << fc.2.ABox.GetNR(fc.2.InAlloc[0]) << "\n"
						f << "store "<< fc.2.MyFuncType.Pars[0].GetName() <<"* %this , "
							<< fc.2.MyFuncType.Pars[0].GetName()  <<"** %SetThis\n"
					}
					prevL = fc.1
				}

				for prS : StolenParams, L : 0
				{
					asMNR := -1 
					for it : funcsUp, F : 0
					{
						asMNR = it.0.GetNR(prS.inAllocId)
						if asMNR == -1
							continue
						f << "%Pre2Get" << ItId << "L" << L << " = load " << prS.ResultType.GetName() << " , " << prS.ResultType.GetName()
							<< "* %T" << prS.inAllocId << "\n"
						f << "%PreSet" << ItId << "L" << L << " = getelementptr %FatLambdaType"<< ItId << " , " 
							<< "%FatLambdaType"<< ItId << "* %PreApply , i32 0, i32 " << F << ",i32 " << asMNR << "\n"
						f << "store " << prS.ResultType.GetName() << " %Pre2Get" << ItId << "L" << L << " , "
							<< prS.ResultType.GetName() << "* %PreSet" << ItId << "L" << L << "\n"
					}

				}
				
				iii := 0
				for it,i : CaptureParams
				{
					if it.1 != null
					{	
						newIdd := GetNewId()
						sii := CPIndexes[iii]
						if it.2 
						{
							f << "%TSt" << sii << " = getelementptr %FatLambdaType" << ItId
								<< " , %FatLambdaType"<< ItId  << "* %PreApply, i32 0,i32 0,i32 "<< cptInAllocNR <<",i32 " << sii << "\n"
							it.1.PrintPointPre(f,newIdd)
							f << "store "
							it.1.PrintPointUse(f,newIdd)
							f << " , " << it.1.ResultType.GetName() << "** "
							f << "%TSt" << sii << "\n"
						}else{
							f << "%TSt" << sii << " = getelementptr %FatLambdaType" << ItId
								<< " , %FatLambdaType"<< ItId  << "* %PreApply, i32 0,i32 0,i32 "<< cptInAllocNR <<",i32 " << sii << "\n"
							it.1.PrintPre(f,newIdd)
							f << "store "
							it.1.PrintUse(f,newIdd)
							f << " , " << it.1.ResultType.GetName() << "* "
							f << "%TSt" << sii << "\n"
						}
						iii += 1
					}
				}
				
				f << "%RRes"<< ItId << " = bitcast "<< ResultType->{TypeFuncLambda^}.GetPointName() << "* %PreSetPoint" << ItId << " to i8*\n"
				f << "ret i8* %RRes"<< ItId << "\n"
				f << "ret i8* null\n"
				f << "}\n"
			}
			if justFunc and Yodlers.Size() != 0
			{
				f << "@Yodler" << ABox.ItId << " = global i32 0\n"
			}
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
			
			
			if not justFunc
			{
				PrintInhers(f,Names[0])
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
			if Yodlers.Size() != null
			{
				if justFunc
				{
					f << "%Yodler = getelementptr i32 , i32* @Yodler" << ABox.ItId << ",i32 0\n"
				}else{
					upABox := funcsUp[0].0
					f << "%Yodler = getelementptr " << upABox.GetClassName() << " , " 
						<< upABox.GetClassName() << "* %Lambda0Box, i32 0, i32 " << ItNR << ",i32 3\n"
				}
				f << "%StartYield = load i32, i32* %Yodler\n"
				f << "switch i32 %StartYield, label %Yield0 ["
				for i : Yodlers.Size() + 1
					f << "i32 " << i << ", label %Yield" << i << "\n"
				f << "]\n"
				f << "Yield0:\n"
			}

			Down.PrintInBlock(f) // MAIN DATA

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
			f << "%Tpl2" << ItId << " = getelementptr " << tupleItem.ClassType.GetName() << "," << tupleItem.ClassType.GetName() << "* %T" << inAlloc << ", i32 0,i32 0\n"
			f << "store " << asL.GetPointName() << " @lambda" << ItId << ", " << ResultType.GetName() << " %Tpl2" << ItId << "\n"
			f << "%TplCnt" << ItId << " = getelementptr " << tupleItem.ClassType.GetName() << "," << tupleItem.ClassType.GetName() << "* %T" << inAlloc << ", i32 0,i32 1\n"
			f << "store i8*(i8*)* @LambdaCopy"<< ItId << ", i8*(i8*)** %TplCnt" << ItId << "\n"
			if Yodlers.Size() != 0
			{
				f << "%TpY" << ItId << " = getelementptr " << tupleItem.ClassType.GetName() << "," << tupleItem.ClassType.GetName() << "* %T" << inAlloc << ", i32 0,i32 3\n"
				f << "store i32 0, i32* %TpY" << ItId << "\n"
			}

			k := 0
			for it,i : CaptureParams
			{
				if it.1 != null
				{
					nwId := GetNewId()
					if it.2
					{
						f << "%StP" << nwId << " = getelementptr " << captureType.GetName() << " , "
							<< captureType.GetName() << "* %T" << cptInAlloc << ", i32 0, i32 " << CPIndexes[k] << "\n"
						it.3.PrintPointPre(f,nwId)
						f << "store "
						it.3.PrintPointUse(f,nwId)
						f << " , "
						f << it.1.ResultType.GetName() << "** "
						f << "%StP" << nwId
						f << "\n"
					}else{
						f << "%StP" << nwId << " = getelementptr " << captureType.GetName() << " , "
							<< captureType.GetName() << "* %T" << cptInAlloc << ", i32 0, i32 " << CPIndexes[k] << "\n"
						it.3.PrintPre(f,nwId)
						f << "store "
						it.3.PrintUse(f,nwId)
						f << " , "
						f << it.1.ResultType.GetName() << "* "
						f << "%StP" << nwId
						f << "\n"

					}
					k++
				}
			}
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
			f << " %Tpl2" << ItId
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
	ApplyFunc := !() -> void
	{
		WorkBag.Push(this&,State_PrePrint)
		applyed = true
		WorkBag.Push(Down,State_Start)
		WorkBag.Push(this&,State_PostGetUse)
	}
	ApplyFunc := !(Type^ lambTyp, bool isFnc) -> void
	{
		WorkBag.Push(this&,State_PrePrint)
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
		for it,i : CaptureParams
		{
			if it.0 == name
			{
				if it.1 == null
				{
					inUp := GetItem(name,Up)
					if inUp? is ObjParam and inUp.Down? is LocalParam
					{
						preRet := new FuncParam("T"sbt + ABox.ItId + "l" + i,inUp.Down.GetType(),true)
						it.1 = preRet
						it.3 = inUp.Down->{MemParam^}
						return preRet
					}else{
						EmitError("can not capture parameter "sbt + name)
					}
				}
				if it.1 != null
					return it.1
				break
			}
		}
		for i : fastUse.ParsCount 
		{
			if Names[i] == name
			{
				return parms[i]
			}
		}

		if not StolenParams.Contain(name)
		{
			inUp := GetItem(name,Up)
			if inUp? is ObjParam and inUp.Down? is LocalParam
			{
				StolenParams[name] = inUp.Down->{LocalParam^}
			}
		}

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

BuiltInLambdaCall := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "()"
		IsMethod = true

		pars := Queue.{Type^}()

		MyFuncType = null
	}
	CreateFuncPointer := virtual !(FuncInputBox itBox) -> TypeFunc^
	{
		pars := ref itBox.itPars

		asL := pars[0].first->{TypeFuncLambda^}
		asB :=  ((pars[0].first.Base)->{TypeFunc^})

		Pars := Queue.{Type^}() ; $temp

		for i : pars.Size()
		{
			if i == 0{
				Pars.Push(pars[0].first)
			}else{
				Pars.Push(asB.Pars[i])
			}
		}
		return GetFuncType(Pars,asB.ParsIsRef,asB.RetType,asB.RetRef,asB.IsVArgs)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars

		if pars.Size() == 0 return 255
		if not pars[0].first is TypeFuncLambda return 255

		asL := pars[0].first->{TypeFuncLambda^}
		asB := ((pars[0].first.Base)->{TypeFunc^})

		if asB.ParsCount != pars.Size() return 255

		maxCmp := 0
		for i : pars.Size()
		{
			if i > 0
			{
				nowCmp := TypeCmp(pars[i].first,asB.Pars[i])
				if maxCmp < nowCmp maxCmp = nowCmp					
			}
		}
		return maxCmp
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ fun) -> BoxFunc^
	{
		pars := ref itBox.itPars

		asL := pars[0].first->{TypeFuncLambda^}
		asB := ((pars[0].first.Base)->{TypeFunc^})

		IsCompl := false

		if not asB.RetRef {
			IsCompl = IsComplexType(asB.RetType)	
		}

		ToSet := "%Wut## = bitcast "sbt+ pars[0].first.GetName() + " #1 to i8*\n"
			+ "%Func## = load " +pars[0].first.Base.GetName() + "* , " + pars[0].first.GetName() + " #1 #d\n"


		if IsCompl or asB.RetType == GTypeVoid{
			ToSet << "call "
		}else{
			ToSet << "#0 = call " 
		}
		ToSet << pars[0].first.Base.GetName() << "%Func##(i8*  %Wut##"

		if IsCompl
		{
			ToSet << " , " << asB.RetType.GetName() << "* #0"
		}

		for i : pars.Size()
		{
			if i != 0{
			ToSet << " , "

				ToSet << asB.Pars[i].GetName()
				if asB.ParsIsRef[i]{
					ToSet << "*"
				}
				ToSet << " #" << (i + 1)
			}
		}
		ToSet << ") #d\n"
		
		return new BuiltInFuncMega("()",fun,ToSet.Str())
	}
}
BuiltInLambdaCall := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "()"
		IsMethod = true

		pars := Queue.{Type^}()

		MyFuncType = null
	}
	CreateFuncPointer := virtual !(FuncInputBox itBox) -> TypeFunc^
	{
		pars := ref itBox.itPars

		asL := pars[0].first->{TypeFuncLambda^}
		asB :=  ((pars[0].first.Base)->{TypeFunc^})

		Pars := Queue.{Type^}() ; $temp

		for i : pars.Size()
		{
			if i == 0{
				Pars.Push(pars[0].first)
			}else{
				Pars.Push(asB.Pars[i])
			}
		}
		return GetFuncType(Pars,asB.ParsIsRef,asB.RetType,asB.RetRef,asB.IsVArgs)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars

		if pars.Size() == 0 return 255
		if not pars[0].first is TypeFuncLambda return 255

		asL := pars[0].first->{TypeFuncLambda^}
		asB := ((pars[0].first.Base)->{TypeFunc^})

		if asB.ParsCount != pars.Size() return 255

		maxCmp := 0
		for i : pars.Size()
		{
			if i > 0
			{
				nowCmp := TypeCmp(pars[i].first,asB.Pars[i])
				if maxCmp < nowCmp maxCmp = nowCmp					
			}
		}
		return maxCmp
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ fun) -> BoxFunc^
	{
		pars := ref itBox.itPars

		asL := pars[0].first->{TypeFuncLambda^}
		asB := ((pars[0].first.Base)->{TypeFunc^})

		IsCompl := false

		if not asB.RetRef {
			IsCompl = IsComplexType(asB.RetType)	
		}

		ToSet := "%Wut## = bitcast "sbt+ pars[0].first.GetName() + " #1 to i8*\n"
			+ "%Func## = load " +pars[0].first.Base.GetName() + "* , " + pars[0].first.GetName() + " #1 #d\n"


		if IsCompl or asB.RetType == GTypeVoid{
			ToSet << "call "
		}else{
			ToSet << "#0 = call " 
		}
		ToSet << pars[0].first.Base.GetName() << "%Func##(i8*  %Wut##"

		if IsCompl
		{
			ToSet << " , " << asB.RetType.GetName() << "* #0"
		}

		for i : pars.Size()
		{
			if i != 0{
			ToSet << " , "

				ToSet << asB.Pars[i].GetName()
				if asB.ParsIsRef[i]{
					ToSet << "*"
				}
				ToSet << " #" << (i + 1)
			}
		}
		ToSet << ") #d\n"
		
		return new BuiltInFuncMega("()",fun,ToSet.Str())
	}
}
BuiltInTemplateSetLambda := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "="
		OutputName = "error"

		emptType := Queue.{Type^}() ; $temp
		emptType.Push(null->{Type^})
		emptType.Push(null->{Type^})
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		if pars.Size() != 2 return 255
		if not pars[0].second return 255
		if pars[0].first != pars[1].first return 255
		if pars[0].first.GetType() != "lambda" return 255
		return 0
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ funct) -> BoxFunc^
	{
		pars := ref itBox.itPars
		return new BuiltInFuncBinar("=",pars[0].first,true,pars[1].first,false,GTypeVoid, 
		"store "sbt + pars[0].first.GetName() + " #2, " + pars[0].first.GetName() +"* #1 #d\n")
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
BuiltInTemplateCaptureLambda := class extend BoxTemplate
{
	this := !() -> void
	{
		IsMethod = true
		FuncName = "Capture"
		OutputName = "error"

		emptType := Queue.{Type^}() ; $temp
		emptType.Push(null->{Type^})
		MyFuncType = GetFuncType(emptType,null->{bool^},GTypeVoidP,false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		if pars.Size() != 1 return 255
		if not pars[0].second return 255
		if pars[0].first.GetType() != "lambda" return 255
		return 0
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ funct) -> BoxFunc^
	{
		pars := ref itBox.itPars
		asPtrN := pars[0].first.Base.GetPoint().GetName()
		return new BuiltInFuncUno(". not",pars[0].first,false,pars[0].first, 
		"%Nxt## = getelementptr "sbt +asPtrN+ " , " + asPtrN +"*  #1 ,i32 1#d\n" +
		"%PreCl## = load "+asPtrN+" , "+asPtrN+"* %Nxt## \n" +
		"%AlmCl## = bitcast "+asPtrN+" %PreCl## to "+asPtrN+"*("+asPtrN+"*)*\n"+
		"#0 = call "+asPtrN+"*%AlmCl##("+asPtrN+"* #1)\n" )
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
BuiltInTemplateDeleteLambda := class extend BoxTemplate
{
	this := !() -> void
	{
		IsMethod = true
		FuncName = "Delete"
		OutputName = "error"

		emptType := Queue.{Type^}() ; $temp
		emptType.Push(null->{Type^})
		MyFuncType = GetFuncType(emptType,null->{bool^},GTypeVoidP,false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		if pars.Size() != 1 return 255
		if not pars[0].second return 255
		if pars[0].first.GetType() != "lambda" return 255
		return 0
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ funct) -> BoxFunc^
	{
		pars := ref itBox.itPars
		asPtrN := pars[0].first.Base.GetPoint().GetName()
		return new BuiltInFuncUno(". not",pars[0].first,false,GTypeVoid, 
		"%Nxt## = getelementptr "sbt+asPtrN+" ,"+asPtrN+"*  #1 ,i32 2#d\n"sbt +
		"%PreCl## = load "+asPtrN+" ,"+asPtrN+"* %Nxt## \n" +
		"%AlmCl## = bitcast "+asPtrN+" %PreCl## to void("+asPtrN+"*)*\n"+
		"call void %AlmCl##("+asPtrN+"* #1)\n" )
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}

copyLFuncType := Type^
copyLFuncTypeP := Type^
deleteLFuncType := Type^
deleteLFuncTypeP := Type^
CreateLambdaBuilts := !() -> void
{
	BuiltInTemplates.Push(new BuiltInLambdaCall())
	BuiltInTemplates.Push(new BuiltInTemplateSetLambda())
	BuiltInTemplates.Push(new BuiltInTemplateCaptureLambda())
	BuiltInTemplates.Push(new BuiltInTemplateDeleteLambda())

	inpFuncE := Queue.{Type^}() ; $temp
	inpFuncE.Push(GTypeVoidP)
	
	copyLFuncType = GetFuncType(inpFuncE,null,GTypeVoidP,false,false)
	copyLFuncTypeP = copyLFuncType.GetPoint()

	deleteLFuncType = GetFuncType(inpFuncE,null,GTypeVoid,false,false)
	deleteLFuncTypeP = deleteLFuncType.GetPoint()
}
