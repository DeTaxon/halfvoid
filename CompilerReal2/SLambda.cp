SLambda := class extend BoxFuncContainer
{
	parsedStart := bool
	applyed := bool
	Names := string[]
	parms := LocalParam^^
	InAlloc := int^
	fastUse := TypeFunc^
	ItId := int
	ItNR := int
	inAlloc := int
	manSkob := bool
	justFunc := bool
	boostLambda := bool

	Created := bool

	StolenParams := AVLMap.{char^,LocalParam^}

	CaptureParams := List.{Tuple.{char^,MemParam^,bool,MemParam^}}
	CPIndexes := List.{int}
	ResultType := Type^
	GetType := virtual !() -> Type^ { return ResultType }
	
	GetScope := virtual !() -> int { return ABox.ItId }
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
		if pri == State_Start and (not parsedStart)
		{
			//AllocSetStruct(Up)
			if boostLambda
			{
				MakeItBlock(Down)
				empt := Queue.{Type^}()
				asFunc := GetFuncType(empt,null->{bool^},GTypeVoid,false,false)
				ResultType = asFunc.GetLambda()
			}else{

				parsedStart = true
				justFunc = Down.Right.GetValue() == "=>"
				//WorkBag.Push(this&,State_Syntax)
				names := Queue.{string}() ; $temp

				pars := Queue.{Type^}() ; $temp
				isRef := Queue.{bool}() ; $temp

				if not justFunc{
					names.Push(StrCopy("lambdaParam"sbt + ItId))
					pars.Push(GTypeVoidP)
					isRef.Push(false)
				}

				isTmpl := false

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
					//TrimCommas(skobPos)
					iter := skobPos.Down
					bag := List.{Object^}() ; $temp
					while iter != null
					{
						if iter.GetValue() != ","
						{
							bag.Push(iter)
						}
						if iter.GetValue() == "," or iter.Right == null
						{
							switch bag.Size()
							{
								case 1
									if not bag[0] is ObjIndent
									{
										EmitError("unknown input objecy\n")
										return void
									}
									asN := bag[0]->{ObjIndent^}
									names.Push(asN.MyStr)
									pars.Push(null->{Type^})
									isRef.Push(false)
									isTmpl = true
								case 2
									if not bag[1] is ObjIndent
									{
										EmitError("unknown input object\n")
										return void
									}
									asN2 := bag[1]->{ObjIndent^}
									names.Push(asN2.MyStr)
									itTyp := ParseType(bag[0])
									if itTyp == null
									{
										EmitError("can not parse type\n")
										return void
									}
									pars.Push(itTyp)
									isRef.Push(false)
								case 0
								case void
									//assert(false)
									EmitError("incorrect input of lambda \n"sbt + bag.Size())
							}
							bag.Clear()

						}
						iter = iter.Right
					}
				}else{
					isTmpl = true
					if Down.GetValue() == "~ind"
					{
						asN := Down->{ObjIndent^}
						names.Push(asN.MyStr)
						pars.Push(null->{Type^})
						isRef.Push(false)
					}else{
						EmitError("incorrect input items of lambda\n")
					}
				}
				Names = names.ToArray()

				asFunc := GetFuncType(pars,null->{bool^},GTypeVoid,false,false)
				fastUse = asFunc


				if not isTmpl
				{
					//if not justFunc
					//{
						Created = true
						parms = new LocalParam^[pars.Size()]
						InAlloc = new int[pars.Size()]
						for it,i : pars
						{
							isRf := IsComplexType(it) 
							isRf = isRf or isRef[i]
							if isRf {
								InAlloc[i] = ABox.GetAlloc(it.GetPoint())
							}else{
								InAlloc[i] = ABox.GetAlloc(it)
							}
							parms[i] = new LocalParam(it,InAlloc[i],isRf)
						}
						WorkBag.Push(Down,State_Syntax)
					//}
					WorkBag.Push(this&,State_PrePrint)
				}

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
		}
		if pri == State_PostGetUse
		{
			//datRes := new FuncInputBox ; $temp
			//datRes.itPars.Emplace(ResultType.Base.GetPoint(),false)

			//tupleItem = GetTuple(datRes)
			inAlloc = GetAlloc(Up,ResultType.Base.GetPoint())
			ItNR = GetAllocNR(Up,inAlloc)
		}
		if pri == State_PrePrint
		{
			ApplyCaptures()
			if Yodlers.Size() != 0
			{
				yodlerInAllocId := ABox.GetAlloc(GTypeInt)
				yodlerInAlloc = ABox.GetNR(yodlerInAllocId)
				if justFunc
				{
					ABox.liveOnGlobal = true
				}else{
					allFnd := Up
					while allFnd != null
					{
						if allFnd.GetValue() == "!()" or allFnd is SLambda
						{
							if allFnd is SLambda
							{
								asL := allFnd->{SLambda^}
								ABox.MoveTo(asL.ABox&)
							}else{
								asF := allFnd->{BoxFuncBody^}
								ABox.MoveTo(asF.ABox&)
							}
							break
						}
						allFnd = allFnd.Up
					}
				}
			}
		}
	}
	PrintInhers := !(sfile f,char^ startS,bool printDeb) -> void
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
			f << "%LBegin" << nameIter << "Pos = getelementptr " << ABName << " , " << ABName<< "* null ,i32 0, i32 " << prevLam.ItNR << "\n"
			f << "%LS2" << nameIter << " = ptrtoint " << prevLam.ResultType.GetName() << " %LBegin" << nameIter << "Pos to i64\n"
			f << "%LS1" << nameIter << " = ptrtoint i8* %"<< prevLName  << " to i64\n"
			f << "%Lambda" << nameIter << "Pre2 = sub i64 %LS1" << nameIter << " , %LS2" << nameIter << "\n"
			f << "%Lambda" << nameIter << "Box = inttoptr i64 %Lambda" << nameIter << "Pre2 to " << ABName << "*\n"
			it.0.PrintBoxItems(f,"%Lambda"sbt + nameIter + "Box",-1) //TODO: add dbg data
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
		if printDeb 
		{
			//funcsUp.Back().2.PrintDebugDeclare(f,this&)
			if Down != null Down[^].PrintDebugDeclare(f,null)
			for it : StolenParams it.PrintDebugDeclare(f,this&)
			
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
		if applyed
		{
			ABox.PrintGlobal(f)
		}
		Down.PrintGlobal(f)
		if applyed
		{
			
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
				io := 0
				for bx : funcsUp
				{
					if io != 0
						f << ","
					if bx.0.parentAlloc == null
					{
						f << bx.0.GetClassName()
						io += 1
					}
				}
				f << "}\n"


				prevLambd := this&
				f << "define void @LambdaDelete" << ItId << "(i8* %ToDel" << ItId << ")\n"
				f << "{\n"
					ABName3 := funcsUp[0].0.GetClassName()
					f << "%LBegin" << nameIter << "Pos = getelementptr " << ABName3 << " , " << ABName3 << "* null ,i32 0, i32 " << prevLambd.ItNR << "\n"
					f << "%LS2" << nameIter << " = ptrtoint " << ResultType.GetName() << " %LBegin" << nameIter << "Pos to i64\n"
					f << "%LS1" << nameIter << " = ptrtoint i8* %ToDel"<< ItId  << " to i64\n"
					f << "%Lambda" << nameIter << "Pre2 = sub i64 %LS1" << nameIter << " , %LS2" << nameIter << "\n"
					f << "%Lambda" << nameIter << "Box = inttoptr i64 %Lambda" << nameIter << "Pre2 to i8*\n"
					f << "call void @free(i8* %Lambda" << nameIter << "Box)\n"
				f << "ret void\n"
				f << "}\n"

				prevLName = ("ToCpy"sbt + ItId <-)

				prevLambd = this&

				f << "define i8* @LambdaCopy" << ItId << "(i8* %ToCpy" << ItId << ") \n"

				f << "{\n"
				PrintInhers(f,"ToCpy"sbt + ItId,false)
				
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
				f << "%PreSetPoint" << ItId << " = getelementptr " << ABName << " , " << ABName << "* %PreSet" << ItId << " , i32 0, i32 " << ItNR << "\n"
				f << "store " << asL.GetPointName() << " @lambda" << ItId << ", " << ResultType.GetName() << " %PreSetPoint" << ItId << "\n"

				//if Yodlers.Size() != 0
				//{
				//	f << "%Yodler = getelementptr " << ABox.GetClassName() << " , " << ABox.GetAsUse() << " i32 0, i32 " << yodlerInAlloc << "\n"
				//	f << "%PreSetYodler" << ItId << " = getelementptr " << ABName << " , " << ABName << "* %PreSet" << ItId << " , i32 0, i32 " << ItNR << ", i32 3\n"
				//	f << "%YodlerValue = load i32 , i32* %Yodler\n"
				//	f << "store i32 %YodlerValue , i32* %PreSetYodler" << ItId << "\n"
				//}
				
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
								<< j << ", i32 " << fc.1.ItNR << "\n"
						}else{
							prevNR := fc.2.ABox.GetNR(prevL.inAlloc)
							f << "%ToSetP" << j << " = getelementptr %FatLambdaType"<< ItId <<", %FatLambdaType" << ItId << "* %PreApply , i32 0, i32 " 
								<< j << ", i32 " << prevNR << "\n"
						}
						f << "%ToSetPT" << j << " = bitcast " << prevL.ResultType.GetName() << " %ToSetP" << j << " to i8*\n"
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

						is2Ref := false
						if prS is LocalParam and prS->{LocalParam^}.IsRef
							is2Ref = true

						if asMNR == -1
							continue
						if is2Ref
						{
							f << "%Pre2Get" << ItId << "L" << L << " = load " << prS.ResultType.GetName() << "* , " << prS.ResultType.GetName()
								<< "** %T" << prS.inAllocId << "\n"
							f << "%PreSet" << ItId << "L" << L << " = getelementptr %FatLambdaType"<< ItId << " , " 
								<< "%FatLambdaType"<< ItId << "* %PreApply , i32 0, i32 " << F << ",i32 " << asMNR << "\n"
							f << "store " << prS.ResultType.GetName() << "* %Pre2Get" << ItId << "L" << L << " , "
								<< prS.ResultType.GetName() << "** %PreSet" << ItId << "L" << L << "\n"
						}else{
							f << "%Pre2Get" << ItId << "L" << L << " = load " << prS.ResultType.GetName() << " , " << prS.ResultType.GetName()
								<< "* %T" << prS.inAllocId << "\n"
							f << "%PreSet" << ItId << "L" << L << " = getelementptr %FatLambdaType"<< ItId << " , " 
								<< "%FatLambdaType"<< ItId << "* %PreApply , i32 0, i32 " << F << ",i32 " << asMNR << "\n"
							f << "store " << prS.ResultType.GetName() << " %Pre2Get" << ItId << "L" << L << " , "
								<< prS.ResultType.GetName() << "* %PreSet" << ItId << "L" << L << "\n"
						}
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
							it.1.PrintPointPre(f,newIdd,-1) //TODO: replace -1 with lambda debug id
							f << "store "
							it.1.PrintPointUse(f,newIdd,-1) //TODO: replace -1 with lambda debug id
							f << " , " << it.1.ResultType.GetName() << "** "
							f << "%TSt" << sii << "\n"
						}else{
							f << "%TSt" << sii << " = getelementptr %FatLambdaType" << ItId
								<< " , %FatLambdaType"<< ItId  << "* %PreApply, i32 0,i32 0,i32 "<< cptInAllocNR <<",i32 " << sii << "\n"
							it.1.PrintPre(f,newIdd,-1) //TODO: replace -1 with lambda debug id
							f << "store "
							it.1.PrintUse(f,newIdd,-1) //TODO: replace -1 with lambda debug id
							f << " , " << it.1.ResultType.GetName() << "* "
							f << "%TSt" << sii << "\n"
						}
						iii += 1
					}
				}

				if ABox.parentAlloc != null and (not ABox.ItemBag.Empty())
				{
					cntr := ABox.InheritNR()
					nmIn := 0
					for funcsUp
					{
						if it.0 == ABox.parentAlloc
							break
						nmIn += 1
					}
					f << "%ToSetAllc = getelementptr %FatLambdaType" << ItId << " , %FatLambdaType" << ItId
					f << "* %PreApply, i32 0, i32 0,i32 " << cntr << "\n"
					f << "%ItAllocPos = getelementptr " << ABox.parentAlloc.GetClassName() << " , " << ABox.parentAlloc.GetClassName() 
						<< "* %Lambda0Box, i32 0, i32 " << cntr <<"\n" 
					f << "%ItAlloc = load " << ABox.GetClassName() << " , " << ABox.GetClassName() 
						<< "* %ItAllocPos\n" 
					f << "store " << ABox.GetClassName() << " %ItAlloc,  " << ABox.GetClassName() 
						<< "* %ToSetAllc\n"
				}
				
				f << "%RRes"<< ItId << " = bitcast "<< ResultType->{TypeFuncLambda^}.GetPointName() << "* %PreSetPoint" << ItId << " to i8*\n"
				f << "ret i8* %RRes"<< ItId << "\n"
				//f << "ret i8* null\n"
				f << "}\n"
			}

			PrintFuncBodySkobs(f,fastUse,Names,"lambda"sbt + ItId,null->{string},ABox.ItId)
			if not justFunc { f << "prefix %LambdaPrefix { i8*(i8*)* @LambdaCopy" << ItId << " , void(i8*)* @LambdaDelete" << ItId << "}" }
			f << "\n"

			if DebugMode
			{
				f << " !dbg !" << ABox.ItId
			}
			f << "\n{\n"
			
			
			if not justFunc
			{
				PrintInhers(f,Names[0],DebugMode)
			}
			debId := -1
			if DebugMode
				debId = CreateDebugCall(this&)
			ABox.PrintAlloc(f,"%Lambda0Box",debId)
			for i : fastUse.ParsCount
			{
				f << "store "
				f << fastUse.Pars[i].GetName()
				if fastUse.ParsIsRef[i] f << "*"
				f << " %" << Names[i] << " , "
				f << fastUse.Pars[i].GetName()
				if fastUse.ParsIsRef[i] f << "*"
				f << "* %T" << InAlloc[i] << "\n"
				if DebugMode and i != 0
				{
					outId := CreateDbgLocVar(this&,fastUse.Pars[i],Names[i])
					newId := CreateDebugCall(this&)
					if newId != -1 and outId != -1
					{
						f << "call void @llvm.dbg.declare(metadata " << fastUse.Pars[i].GetName()
						if fastUse.ParsIsRef[i] 
							f << "*"
						f <<"* %T" << InAlloc[i] << " , metadata !" << outId << " , metadata !DIExpression()) , !dbg !" << newId << "\n"
					}
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
			if Yodlers.Size() != null
			{
				f << "%Yodler = getelementptr " << ABox.GetClassName() << " , " << ABox.GetAsUse() << " , i32 0, i32 " << yodlerInAlloc << "\n" 
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
				if fastUse.RetRef retTypeName = (""sbt  + retTypeName + "*" <-) 
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
					f << "!" << ABox.ItId << " = distinct !DISubprogram(name:\":LambdaFunction\","
					f << "scope: !" << asN.fileId << " , file: !" << asN.fileId
					f << ",type: !" << fastUse.metaId 
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
	PrintPre := virtual !(sfile f) -> void
	{
		if justFunc return void
		if applyed
		{
			asL := ResultType->{TypeFuncLambda^}
			f << "%Tpl2" << ItId << " = getelementptr " << ResultType.Base.GetPoint().GetName() << "," << ResultType.Base.GetPoint().GetName() << "* %T" << inAlloc << ", i32 0\n"
			f << "store " << asL.GetPointName() << " @lambda" << ItId << ", " << ResultType.GetName() << " %Tpl2" << ItId << "\n"

			if Yodlers.Size() != 0
			{
				inNR := ABox.InheritNR()
				f << "%TpY" << ItId << " = getelementptr " << ABox.parentAlloc.GetClassName() << "," << ABox.parentAlloc.GetAsUse() << " ,i32 0,i32 " << inNR <<", i32 " << yodlerInAlloc <<"\n"
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
						it.3.PrintPointPre(f,nwId,-1) //TODO: replace -1 with lambda debug id
						f << "store "
						it.3.PrintPointUse(f,nwId,-1) //TODO: replace -1 with lambda debug id
						f << " , "
						f << it.1.ResultType.GetName() << "** "
						f << "%StP" << nwId
						f << "\n"
					}else{
						f << "%StP" << nwId << " = getelementptr " << captureType.GetName() << " , "
							<< captureType.GetName() << "* %T" << cptInAlloc << ", i32 0, i32 " << CPIndexes[k] << "\n"
						it.3.PrintPre(f,nwId,-1) //TODO: replace -1 with lambda debug id
						f << "store "
						it.3.PrintUse(f,nwId,-1) //TODO: replace -1 with lambda debug id
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
		if boostLambda
		{
			nams  := Queue.{char^}() ; $temp

			if not justFunc{
				nams.Push("lambdaParam" + ItId)
			}
			lType := lambTyp.Base->{TypeFunc^}
			for i : lType.ParsCount
			{
				if i == 0 continue
				itNN := "_"sbt + i
				nams.Push(itNN.Str())
			}
			Names = nams.ToArray()
			
		}

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

			if asFType.RetType != GTypeVoid and (not isRetCm)
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
					if (inUp? is ObjParam and inUp.Down? is LocalParam) or inUp is LocalParam or inUp is FuncParam
					{
						itMem := MemParam^()
						if inUp is LocalParam or inUp is FuncParam
						{
							itMem = inUp->{MemParam^}
						}else{
							if inUp.Down? is LocalParam
							{
								itMem = inUp.Down->{MemParam^}
							}else{
								assert(false)
							}
						}
						preRet := new FuncParam("T"sbt + ABox.ItId + "l" + i,itMem.GetType(),true)
						it.1 = preRet
						it.3 = itMem
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
		if fastUse != null
		{
			for i : fastUse.ParsCount 
			{
				if Names[i] == name
				{
					return parms[i]
				}
			}
		}

		if not StolenParams.Contain(name)
		{
			inUp := GetItem(name,Up)
			if inUp? is ObjParam and inUp.Down? is LocalParam
			{
				StolenParams[name] = inUp.Down->{LocalParam^}
			}
			if inUp != null and inUp is LocalParam
			{
				StolenParams[name] = inUp->{LocalParam^}
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
	PrintDebugDeclare := virtual !(sfile f ,Object^ frc) -> void
	{
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
		
		return new BuiltInFuncMega("()",fun,ToSet)
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
		"%Fnc## = load "sbt + asPtrN + " , " + asPtrN + "* #1\n" +
		"%Pref## = bitcast " + asPtrN + " %Fnc## to %LambdaPrefix*\n" +
		"%ClFuncPre## = getelementptr %LambdaPrefix , %LambdaPrefix* %Pref##, i32 -1, i32 0\n"+
		"%ClFunc## = load i8*(i8*)*, i8*(i8*)** %ClFuncPre##\n"+
		"%LamPtr## = bitcast " + asPtrN + "* #1 to i8*\n"+
		"%CopyRes## = call i8* %ClFunc##(i8* %LamPtr##)\n" +
		"#0 = bitcast i8* %CopyRes## to " + asPtrN + "*\n")
		//"%Nxt## = getelementptr "sbt +asPtrN+ " , " + asPtrN +"*  #1 ,i32 1#d\n" +
		//"%PreCl## = load "+asPtrN+" , "+asPtrN+"* %Nxt## \n" +
		//"%AlmCl## = bitcast "+asPtrN+" %PreCl## to "+asPtrN+"*("+asPtrN+"*)*\n"+
		//"#0 = call "+asPtrN+"*%AlmCl##("+asPtrN+"* #1)\n" )
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
		"%Fnc## = load "sbt + asPtrN + " , " + asPtrN + "* #1\n" +
		"%Pref## = bitcast "sbt + asPtrN + " %Fnc## to %LambdaPrefix*\n" +
		"%ClFuncPre## = getelementptr %LambdaPrefix , %LambdaPrefix* %Pref##, i32 -1, i32 1\n"+
		"%ClFunc## = load void(i8*)*, void(i8*)** %ClFuncPre##\n"+
		"%LamPtr## = bitcast " + asPtrN + "* #1 to i8*\n"+
		"call void %ClFunc##(i8* %LamPtr##)")
		//"%Nxt## = getelementptr "sbt+asPtrN+" ,"+asPtrN+"*  #1 ,i32 2#d\n"sbt +
		//"%PreCl## = load "+asPtrN+" ,"+asPtrN+"* %Nxt## \n" +
		//"%AlmCl## = bitcast "+asPtrN+" %PreCl## to void("+asPtrN+"*)*\n"+
		//"call void %AlmCl##("+asPtrN+"* #1)\n" )
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
PrintLambdaGlobal := !(sfile f) -> void
{
	f << "%LambdaPrefix = type {i8*(i8*)*,void(i8*)*}\n"
}
