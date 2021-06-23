FillAttrs := !(FuncInputBox itBox,Object^ itm) -> void
{	
	if itm != null{
		for v,k : itm.Line.itAttrs
		{
			//printf("adding %s\n",k)
			itBox.itAttrs[k] = v ; $temp
		}
	}

	itr := itm
	while itr != null
	{
		if itr is BoxFuncBody
			break
		itr = itr.Up
	}
	if itr == null return void

	asFunc := itr->{BoxFuncBody^}

	for val,key : asFunc.ItAttrs
	{	
		if itBox.itAttrs.TryFind(key) == null
			itBox.itAttrs[key] = val ; $temp
	}
}

GetFuncCall := !(Object^ ToParse) -> Object^
{
	iter := ToParse

	if iter == null return null

	if iter is ObjIndent //iter.GetValue() == "~ind"
	{
		asInd := iter->{ObjIndent^}
		box := new FuncInputBox() ; $temp 
		box.itConsts.Push(new ObjStr(asInd.MyStr)) 
		someF := FindFunc(".",iter,box^,true)

		if someF != null
		{
			return MakeSimpleCall2(someF,null->{Object^},iter)
		}
	}

	return FuncCallTryCompute(ToParse)? // const + const => const , directly computing
	return FuncCallSpecific(ToParse)? // Metadata  x->Type, int->Size   this->Fields
	return FuncCallCheckCreateArray(ToParse)? // ![]
	return FuncCallFirstIsType(ToParse)? // constructor()

	iter = ToParse.Down
	if iter == null return null
	
	if iter.GetType() == null
	{
		return FuncCallPrefixOperator(iter)? // new int , delete x , -x
		return null
	}

	iter = iter.Right
	if iter == null return null
	iterPre := iter

	return FuncCallFuncObjectCall(iterPre,iter)? // x() , where x is function pointer or class
	return FuncCallGetArrayElement(iterPre,iter)? // x[]

	return FuncCallSuffix(iter)? // 90deg , 10msec
	return FuncCallGetFieldTypes(iterPre,iter)? // x.y x->y x.y()
	
	return FuncCallOperator(iter)? // x + y , z * 13 ,  a ?: b : c   (no unary minus)
		
	return null
}
OperFunc := !(string oper,Object^ pars,Object^ operNode) -> Object^
{
	assert(operNode != null)
	preRet := OneCall(oper,pars.Up,null->{Object^},true)
	
	if preRet != null
	{
		preRet.Line = operNode.Line
		return preRet
	}

	if pars.GetType() != null
	{
		if pars.GetType() is TypeClass
		{
			asNeedPre := pars.GetType()
			asNeed := asNeedPre->{TypeClass^}
			cls := asNeed.ToClass

			oBox := new FuncInputBox()  ; $temp
			FillAttrs(oBox^,pars)

			
			for iter : pars
			{
				oBox.itPars.Emplace(iter.GetType(),iter.IsRef())
			}
			if pars is MetaItemWrapper
			{
				oBox.itMetaPtr = pars->{MetaItemWrapper^}.ptrToBlock
			}

			newPre := cls.GetFunc(oper,oBox^,true)
			if newPre == null return null
			return MakeSimpleCall2(newPre,pars,operNode)
		}
	}
	return null
}
IsOper := !(string may) -> bool
{
	return OpersTree.Contain(may)
}

TrimCommas := !(Object^ up) -> void
{
	if up.Down != null
	{
		Temp := up
		iter := Temp.Down
		while iter != null
		{
			if iter.GetValue() == ","
			{
				PopOutNode(iter)
				iter = Temp.Down
			}else
			{
				iter = iter.Right
			}
		}
	}
}

OneCall := !(string Name, Object^ G,Object^ consts) -> Object^
{
	return OneCall(Name,G,consts,false)
}
OneCall := !(string Name, Object^ G,Object^ constsPre,bool ignoreNull) -> Object^
{
	Cs := Queue.{Object^}()

	if constsPre != null
	{
		for H : constsPre.Down
		{
			if H.GetValue() != ","
			{
				if H.IsConst
				{
					Cs.Push(H)
				}else{
					tp := ParseType(H)
					if tp == null
					{
						ErrorLog.Push("can not parse type in .{}\n")
					}else{
						Cs.Push(new ObjType(tp)) 
					}
				}
			}
		}
	}

	return OneCall(Name,G,Cs,ignoreNull)
}
OneCall := !(string Name, Object^ G,Queue.{Object^} consts,bool ignoreNull) -> Object^
{
	box := new FuncInputBox()  ; $temp
	FillAttrs(box^,G)
	if G?.Down?.inhAttrs != null
	{
		for v,k : G.Down.inhAttrs^
		{
			if box.itAttrs.TryFind(k) == null
				box.itAttrs[k] = v ; $temp
		}
	}

	TrimCommas(G)
	P := G.Down

	if P? is MetaItemWrapper
	{
		box.itMetaPtr = P->{MetaItemWrapper^}.ptrToBlock
	}

	iterT := P

	while iterT != null
	{
		box.itPars.Emplace(iterT.GetType(),iterT.IsRef()) 
		iterT = iterT.Right
	}
	for it : consts box.itConsts.Push(it) 

	func1 := FindFunc(Name,G,box^,false)

	if Name in !["<",">","<=",">=","==","!="]
	{
		func2 := FindFunc("<=>",G,box^,false)

		if func1 == null
		{
			if func2 != null
			{
				preRet := MakeSimpleCall(func2,P)
				preRet.Line = P.Line
				spFunc := GetSpaceTransformer(Name)
				return MakeSimpleCall2(spFunc,preRet,P)
			}
		}else{
			if func2 != null
			{
				pri1 := ComputePriorFunc(func1.MyFuncType,box^)
				pri2 := ComputePriorFunc(func2.MyFuncType,box^)
				if pri2 < pri1
				{
					preRet := MakeSimpleCall(func2,P)
					preRet.Line = P.Line
					spFunc := GetSpaceTransformer(Name)
					return MakeSimpleCall2(spFunc,preRet,P)
				}
			}
		}
	}

	if func1 == null 
	{
		if not ignoreNull 
		{
			ErrorLog.Push("Function <"sbt + Name + "> not found\n") //TODO:  PointCall and closestFunc
		}
	}
	else
	{
		return MakeSimpleCall(func1,P)
	}
	return null	
}

MakeSimpleCall2 := !(BoxFunc^ func, Object^ pars,Object^ lineObj) -> BaseFuncCall^
{
	fCall := MakeSimpleCall(func,pars)
	if lineObj != null
		fCall.Line = lineObj.Line
	return fCall
}
MakeSimpleCall := !(BoxFunc^ func, Object^ pars) -> BaseFuncCall^
{
	return func.GenerateCall(pars)?
	if func.IsAssembler() return new AssemblerCall(func,pars)
	return new NaturalCall(func,pars)
}

BaseFuncCall := class extend ObjResult
{
	ToCall := BoxFunc^
	FType := TypeFunc^

	GetType := virtual !() -> Type^
	{
		return FType.RetType
	}
	PrintGlobal := virtual !(TIOStream f) -> void
	{
		iter := Down
		while iter != null
		{
			iter.PrintGlobal(f)
			iter = iter.Right
		}
	}
	GetValue := virtual !() -> string
	{
		return "d()"
	}
	ExchangeParams := !() -> void
	{
		itLiner := this.Line
		lnr := this&->{Object^}

		if itLiner == null{
			wl := Down
			while wl != null{
				if wl.Line != null {
					itLiner = wl.Line
					lnr = wl
				}
				wl = wl.Right
			}
		}

		iter := Down
		//if ToCall != null 
		//	if ToCall.IsRetComplex 
		//		iter = iter.Right //???
		i := 0

		while iter != null and i < FType.ParsCount 
		{
			if iter.GetType() != FType.Pars[i]
			{
				RetR := false
				if FType.ParsIsRef != null
					RetR = FType.ParsIsRef[i]
				preRet := BoxExc(iter,FType.Pars[i],RetR)
	
				if preRet == null
				{
					msg := "compiler bug at param "sbt + (i+1) + " "
					msg << " object " << iter.GetValue()
					if iter.GetType() != null{
						msg << " from " << iter.GetType().GetName()
					}else {
						msg << " from null "
					}
					if FType.Pars[i] != null {
						msg << " to "  <<  FType.Pars[i].GetGoodName() 
					}
				
					if itLiner != null lnr.EmitError(msg + "\n") else assert(false)
				}else{
					//if preRet.Line == null 
					//{
					//	Print(0)
					//}
					iter = preRet
				}
			}else
			{
				if IsSLambda(iter) iter->{SLambda^}.ApplyFunc()
			}
			i += 1
			iter = iter.Right
		}
		while iter != null
		{
			itType := iter.GetType()
			if itType != null
			{
				if itType == GTypeFloat or itType == GTypeHalf
				{
					iter = BoxExc(iter,GTypeDouble,false)
				}
				if itType is TypeArr
				{
					iter = BoxExc(iter,iter.GetType().Base.GetPoint(),false)
				}
				if itType is TypeClass
				{
					iter = BoxExc(iter,GTypeString,false)
					if iter == null
					{
						EmitError("variadic arg can not be casted to string")
					}
				}
			}else{
				if iter.GetValue() != ","
				{
					EmitError("can not parse param in variadic args ")
				}
			}
			iter = iter.Right
		}
	}
}

SomeFuncCall := class extend BaseFuncCall //TODO TODO TODO: REFACTOR 
{
	RetId := int

	IsConstr := bool
	gotAlloc := bool
	InAlloc := int

	TName := string
	TEName := string


	UseCall := virtual !(TIOStream f) -> void
	{
	}
	PrintPointPre := virtual !(TIOStream f) -> void
	{
		UseCall(f)
	}
	IsRef := virtual !() -> bool
	{
		if gotAlloc return true
		if FType == null {
			EmitError("compiler error?\n")
			Print(0)
			return false
		}
		if FType.RetRef return true
		return false
	}

	checkedReturn := bool
	CheckReturn := virtual !() -> void
	{
		if checkedReturn return void
		checkedReturn = true
		if ToCall != null and not gotAlloc
		{
			gotAlloc = ToCall.IsRetComplex

			if ToCall.MyFuncType.RetType != null
			{
				if not gotAlloc gotAlloc = ToCall.MyFuncType.RetType is TypeClass
				if not gotAlloc gotAlloc = ToCall.MyFuncType.RetType is TypeArr
			}
			if ToCall.IsRetRef gotAlloc = false
			if ToCall.MyFuncType.RetRef gotAlloc = false //TOD: WHAT THE FUK?
			if gotAlloc
			{
				if Up.GetValue() == "~Return()"
				{
					TName = StrCopy("%"sbt + ReturnName)
				}else{
					InAlloc = GetAlloc(this&,ToCall.MyFuncType.RetType)
					TName = StrCopy("%T"sbt + InAlloc)
				}
				TEName = StrCopy("%TE"sbt + RetId)
			}else
			{
				TName = StrCopy("%T"sbt + RetId)
				TEName = StrCopy("%TE"sbt + RetId)
			}
		}else{
			if not gotAlloc
			{
				TName = StrCopy("%T"sbt + RetId)
				TEName = StrCopy("%TE"sbt + RetId)
			}
		}
	}
	PrintPre := virtual !(TIOStream f) -> void
	{
		UseCall(f)
		if ToCall != null //TODO: ??? ToCall can not be null 
		{
			if FType.RetRef or ToCall.IsRetRef
			{
				f << TEName << " = load "
				f << FType.RetType.GetName()
				f << " , "
				f << FType.RetType.GetPoint().GetName()
				f << " " << TName << "\n"
			}
		}
	}
	PrintPointUse := virtual !(TIOStream f) -> void
	{
		FType.RetType.GetPoint().PrintType(f)
		f << " " << TName
	}
	PrintUse := virtual !(TIOStream f) -> void
	{
		FType.RetType.PrintType(f)
		if ToCall != null
		{
			if FType.RetRef or ToCall.IsRetRef
			{
				f << " " << TEName
			}else{
				f << " " << TName
			}
		
		}else{
			f << " " << TName
		}
	}
	GetItAllocId := !() -> int
	{
		if not gotAlloc return -1
		return InAlloc
	}
	GetPointName := virtual !() -> string
	{
		return TName
	}
	GetName := virtual !() -> string
	{
		if ToCall != null
		{
			if FType.RetRef or ToCall.IsRetRef
			{
				return TEName
			}
		}
		return TName
	}
	GetOutputName := virtual !() -> string
	{
		return TName
	}
	PrintInBlock := virtual !(TIOStream f) -> void
	{
		PrintPre(f)
	}
	GetType := virtual !() -> Type^
	{
		return FType.RetType
	}
}

NaturalCall := class extend SomeFuncCall
{
	gcObjId := int
	this := !(BoxFunc^ func, Object^ Pars) -> void 
	{
		if Pars != null
		{
			Line = Pars.Line
		}
		if func.IsPassAttrs and Pars != null
		{
			inhAttrs = Pars.inhAttrs
		}
		Down = Pars
		
		RetId = GetNewId()
		ToCall = func
		ToCall.ParseBlock()
		FType = ToCall.MyFuncType
		if Pars != null Pars.SetUp(this&)
		if Pars != null TrimCommas(Down.Up)
		ExchangeParams()
		WorkBag.Push(this&,State_MiddleGetUse)
	}
	
	PrintPreFuncName := virtual !(TIOStream f) -> void
	{

	}
	PrintFuncName := virtual !(TIOStream f) -> void
	{
		if ToCall != null
		f << "@" << ToCall.OutputName
	}

	PrintParamPres := virtual !(TIOStream f) -> void
	{
		RefsArr := FType.ParsIsRef
		for iter,i : Down
		{
			if RefsArr[i] iter.PrintPointPre(f)
			else iter.PrintPre(f)
		}
	}
	PrintParamUses := virtual !(TIOStream f) -> void
	{
		iter := Down
		RefsArr := FType.ParsIsRef

		i := 0
		if gotAlloc and not IsConstr
		{
			f << ToCall.MyFuncType.RetType.GetName() << "* "
			f << TName
			if iter != null f << " , "
			//i += 1
		}
		while iter != null
		{
			if i > 0  f << " , "
			if RefsArr[i] iter.PrintPointUse(f)
				else iter.PrintUse(f)
			iter = iter.Right
			i += 1
		}
	}

	UseCall := virtual !(TIOStream f) -> void
	{
		PrintPreFuncName(f)
		PrintParamPres(f)

		if (FType.RetType != GTypeVoid and (not gotAlloc) and TName != null) or ToCall.IsRetRef
		{
			f << TName <<" = "	
		}
		f << "call "
		//ToCall.MyFuncType.PrintType(f)
		base := ToCall.MyFuncType
		base2 := base->{Type^}
		f << base2.GetName()
		PrintFuncName(f)
		f << "("
		PrintParamUses(f)
		f << ")"
		if DebugMode {
			newId := CreateDebugCall(this&) 
			if newId != -1{
				f << ", !dbg !" << newId
			}
		}
		if Line != null
		{
			f << "; Line: " << Line.LinePos << " File: " << Line.inFile.itStr 

		}
		f << "\n"
		if gcObjId > 0
		{
			tn := FType.RetType.GetName() 
			f << "store " << tn << TName << " , "
			f << tn << "* %T" << gcObjId << "\n"
		}
	}
	Print := virtual !(int s) -> void {
		for s printf("->")
		printf("item: %s %s\n",GetValue(),ToCall.FuncName)
		End := this.Down
		while End != null
		{
			End.Print(s+1)
			End = End.Right
		}
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_MiddleGetUse
		{
			CheckReturn()
			if IsGCAnyPtr(FType.RetType) and not FType.RetRef
			{
				gcObjId = GetAlloc(this&,FType.RetType)
				assert(gcObjId > 0)
				GCMakeAware(this&,gcObjId)
			}
		}
	}
}

PointFuncCall := class extend NaturalCall
{
	ParamCal := Object^
	this := !(TypeFunc^ funcT, Object^ Pars, Object^ pCall) -> void 
	{
		Down = Pars
		
		RetId = GetNewId()
		FType = funcT
		if Pars != null Pars.SetUp(this&)
		ParamCal = pCall
		ExchangeParams()
		WorkBag.Push(this&,State_MiddleGetUse)
	}
	PrintInBlock := virtual !(TIOStream f) -> void
	{
		UseCall(f)
	}
	UseCall := virtual !(TIOStream f) -> void
	{
		if compilerSuffix == "posix"
		{
			containAbi := false
			for i : FType.ParsCount
			{
				if (not FType.ParsIsRef[i]) and FType.Pars[i] is TypeClass
				{
					containAbi = true
					break
				}
			}
			if containAbi
			{
				UseLinuxAbiCall(f)
				return void
			}
		}
		PrintPreFuncName(f)
		PrintParamPres(f)
		ParamCal.PrintPre(f)

		if (FType.RetType != GTypeVoid)
		{
			f << "%T" << RetId <<" = "	
		}
		f << "call "
		FType.PrintType(f)
		f << ParamCal.GetName()
		f << "("
		PrintParamUses(f)
		f << ")"
		if DebugMode
		{
			newId := CreateDebugCall(this&)
			if newId != -1
				f << " , !dbg !" << newId
		}
		f << "\n"
	}
	UseLinuxAbiCall := virtual !(TIOStream f) -> void 
	{

		PrintPreFuncName(f)
		needComma := false

		funcTypeNameTmpl := ""sbt
		funcTypeNameTmpl <<  FType.RetType.GetName() << "("

		for it,i : Down
		{
			if i >= FType.ParsCount
			{
				it.PrintPre(f)
				continue
			}
			if FType.Pars[i] is TypeClass and not FType.ParsIsRef[i]
			{
				asClass := FType.Pars[i]->{TypeClass^}.ToClass
				it.PrintPointPre(f)
				
				if needComma funcTypeNameTmpl << ","
				needComma = true
				cn := it.GetType().GetName()
				//funcTypeNameTmpl << cn << "* byval(" << cn << ")"
				funcTypeNameTmpl << cn << "* "

			}else{
				if needComma funcTypeNameTmpl << ","
				needComma = true
				if FType.ParsIsRef[i] {
					funcTypeNameTmpl << FType.Pars[i].GetName() << "*"
					it.PrintPointPre(f)
				}else{
					funcTypeNameTmpl << FType.Pars[i].GetName()
					it.PrintPre(f)
				}
			}
		}
		funcTypeNameTmpl << ")"
		fName := funcTypeNameTmpl.Str() ; $temp
		
		ParamCal.PrintPre(f)
		f << "%TId" << RetId << " = bitcast " ParamCal.PrintUse(f) f << " to " << fName << "*\n"

		needComma = false
		if (FType.RetType != GTypeVoid)
		{
			f << "%T" << RetId <<" = "	
		}
		f << "call "
		f << fName
		f << "%TId" << RetId
		f << "("
		for it,i : Down
		{
			if i >= FType.ParsCount
			{
				it.PrintUse(f)
				continue
			}
			if FType.Pars[i] is TypeClass and not FType.ParsIsRef[i]
			{
				asClass := FType.Pars[i]->{TypeClass^}.ToClass
				
				if needComma f << ","
				needComma = true
				cn := it.GetType().GetName()
				f << cn << "* byval(" << cn << ") "
				f << it.GetPointName()

			}else{
				if needComma f << ","
				needComma = true
				if FType.ParsIsRef[i] {
					it.PrintPointUse(f)
				}else{
					it.PrintUse(f)
				}
			}
		}
		f << ")"
		if DebugMode
		{
			newId := CreateDebugCall(this&)
			if newId != -1
				f << " , !dbg !" << newId
		}
		f << "\n"

	}
	Print := virtual !(int s) -> void {
		for s printf("->")
		printf("item: %s \n",GetValue())
		End := this.Down
		while End != null
		{
			End.Print(s+1)
			End = End.Right
		}
	}
	GetType := virtual !() -> Type^
	{
		return FType.RetType
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_MiddleGetUse
		{
			CheckReturn()
		}
	}
}

AssemblerCall := class extend NaturalCall
{
	RealCall := BuiltInFunc^ at ToCall

	this := !(BoxFunc^ func, Object^ Pars) -> void 
	{
		if func.IsPassAttrs and Pars != null
		{
			inhAttrs = Pars.inhAttrs
		}
		Down = Pars
		RetId = GetNewId()
		ToCall = func
		FType = ToCall.MyFuncType
		if Pars != null Pars.SetUp(this&)
		ExchangeParams()
		WorkBag.Push(this&,State_MiddleGetUse)
	}
	PrintInBlock := virtual !(TIOStream f) -> void
	{
		PrintPre(f)
	}
	GetType := virtual !() -> Type^
	{
		return FType.RetType
	}
	UseCall := virtual !(TIOStream f) -> void
	{
		RealCall.PrePrintEvent()
		PrintPreFuncName(f)
		RefsArr := FType.ParsIsRef

		if not RealCall.IsSelfPre
			PrintParamPres(f)
		
		AsmLine := RealCall.ToExe

		buf := char[2]
		buf[1] = 0

		debId := 0

		thisName := GetName()
		j := 0
		while AsmLine[j] != 0
		{
			if AsmLine[j] != '#'
			{
				buf[0] = AsmLine[j]
				f << buf
				j += 1
			}else{
				j+= 1
				isFullName := false
				if AsmLine[j] == 'T'
				{
					isFullName = true
					j += 1
				}
				if AsmLine[j] in '0'..'9'
				{
					num := 0

					while AsmLine[j] in '0'..'9'
					{
						num *= 10
						num += AsmLine[j] - '0'
						j += 1
					}

					if num == 0
					{
						f << GetOutputName()
					}else{
						num -= 1
						miniIter := Down
						for num miniIter = miniIter.Right

						if miniIter == null
						{
							printf("nope %i %s\n",num,RealCall.FuncName)
							assert(false)
						}
						
						if isFullName
						{
							if RefsArr[num] miniIter.PrintPointUse(f)
							else miniIter.PrintUse(f)
						}else{
							ToAdd := string
							if RefsArr[num] ToAdd = miniIter.GetPointName()
							else ToAdd = miniIter.GetName()

							f << ToAdd
						}
					}
				}
				if AsmLine[j] == 'd'
				{
					j += 1
					if DebugMode {
						if debId == 0
							debId = CreateDebugCall(this&)
						if debId != -1
						{
							f << ", !dbg !" << debId
						}
					}

				}
				if AsmLine[j] == '^'
				{
					numC := 0
					j += 1

					while AsmLine[j] in '0'..'9'
					{
						numC *= 10
						numC += AsmLine[j] - '0'
						j += 1
					}

					iterR := Down
					
					numC -= 1
					k := numC
					while k  != 0
					{
						iterR = iterR.Right
						k -= 1
					}

					if RealCall.MyFuncType.ParsIsRef[numC]
						iterR.PrintPointPre(f)
					else	iterR.PrintPre(f)

				}
				if AsmLine[j] == '#'
				{
					f << RetId
					j += 1
				}
			}
		}

	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_MiddleGetUse
		{
			CheckReturn()
		}
	}
}
BuiltIn2Call := class extend BaseFuncCall
{
	RealCall := BuiltIn2Func^ at ToCall
	thisClId := int

	this := !(BoxFunc^ func, Object^ Pars) -> void 
	{
		if func.IsPassAttrs and Pars != null
		{
			inhAttrs = Pars.inhAttrs
		}
		Down = Pars
		//thisClId = GetNewId() //TODO someone toching my ram
		ToCall = func
		FType = ToCall.MyFuncType
		if Pars != null Pars.SetUp(this&)
		ExchangeParams()
		if FType.RetRef
		{
			cc := this&
			cc->SetType(BuiltIn2CallRef)
		}
	}
	GenId := !() -> int
	{
		thisClId = GetNewId()
		return thisClId
	}
	PrintInBlock := virtual !(TIOStream f) -> void
	{
		PrintPre(f)
	}
	PrintRes := !(TIOStream f) -> void
	{
		f << " %T" << thisClId
	}
	PrintUse := virtual !(TIOStream f) -> void
	{
		FType.RetType.PrintType(f)
		PrintRes(f)
	}
	GetName := virtual !() -> char^
	{
		return "%T"sbt + thisClId
	}
	PrintPre := virtual !(TIOStream f) -> void
	{
		RealCall.PrintFunc(this&,f)
	}
}
BuiltIn2CallRef := class extend BuiltIn2Call
{
	PrintPointUse := virtual !(TIOStream f) -> void
	{
		FType.RetType.GetPoint().PrintType(f)
		PrintRes(f)
	}
	IsRef := virtual !() -> bool
	{
		return true
	}
	GetPointName := virtual !() -> char^
	{
		return "%T"sbt + thisClId
	}
	PrintPointPre := virtual !(TIOStream f) -> void
	{
		RealCall.PrintFunc(this&,f)
	}
	PrintUse := virtual !(TIOStream f) -> void
	{
		FType.RetType.PrintType(f)
		f << "%TR" << thisClId
	}
	GetName := virtual !() -> char^
	{
		return "%TR"sbt + thisClId
	}
	PrintPre := virtual !(TIOStream f) -> void
	{
		PrintPointPre(f)
		rn := FType.RetType.GetName()
		f << "%TR" << thisClId << " = load " << rn << " , "
		PrintPointUse(f)
		f << "\n"
	}
}
TypeSizeCall := class extend SomeFuncCall
{
	ToCmp := Type^
	this := !(Type^ toCmp) ->void
	{
		RetId = GetNewId()
		ToCmp = toCmp
		ResultType = GTypeInt
	}
	PrintPointPre := virtual !(TIOStream f) -> void {	}
	PrintPre := virtual !(TIOStream f) -> void
	{
		f << "%TPre" << RetId << " = getelementptr "<<ToCmp.GetName()<< " , "<<ToCmp.GetPoint().GetName()<< " null, i32 1\n"
		f << "%T" << RetId << " = ptrtoint "<<ToCmp.GetPoint().GetName() << " %TPre" <<RetId<< " to i32\n"
	}
	PrintPointUse := virtual !(TIOStream f) -> void
	{
	}
	PrintUse := virtual !(TIOStream f) -> void
	{
		f << " %T" << RetId
	}
	GetPointName := virtual !() -> string
	{
		return "%T"sbt + RetId
	}
	GetName := virtual !() -> string
	{
		return "%T"sbt + RetId
	}
	GetOutputName := virtual !() -> string
	{
		return StrCopy("%T"sbt + RetId)
	}
	GetType := virtual !() -> Type^
	{
		return ResultType
	}
}


DeleteCall := class extend SomeFuncCall
{
	DestructFuncCall := Object^
	DeleteFuncCall := Object^	
	this := !(Object^ itm) -> void
	{
		PopOutNode(itm)
		Down = itm
		itm.SetUp(this&)
		if not (itm.GetType() is TypePoint or itm.GetType() is TypeFatArr)
		{
			EmitError("only pointer could be deleted\n")
		}else{
			WorkBag.Push(this&,State_GetUse)
			WorkBag.Push(this&,State_PreGetUse)
		}
		Line = itm.Line
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_GetUse
		{
			if IsGCAnyPtr(Down.GetType())
			{
				return void	
			}

			box := new FuncInputBox()  ; $temp


			box.itPars.Emplace(VoidPType,false)
			//box.itConsts.Push(new ObjType(Down.GetType())) 

			func := FindFunc("delete",Up,box^,false)

			if func != null
			{
				func.Line = Line
				DeleteFuncCall = MakeSimpleCall(func,new LinkForThis(Down,Down.GetType(),true))
				DeleteFuncCall.Up = this&
				DeleteFuncCall.Line = this.Line

				//if Down.GetType().Base.GetType() == "class" and Down.GetType().GetType() == "point"
				//{
				//	itTypPre := Down.GetType().Base
				//	itTyp := itTypPre->{TypeClass^}

				//	pars2 := Queue.{Type^}()
				//	pars2.Push(itTypPre)
				//	func2 := itTyp.ToClass.GetFunc("~this",pars2)

				//	if func2 != null
				//	{
				//		halfCall := new LinkForThis(Down,Down.GetType(),true)
				//		ptrCall := new PtrToRef(halfCall->{Object^})
				//		DestructFuncCall = MakeSimpleCall(func2,ptrCall)
				//		DestructFuncCall.Up = this&
				//	}else{
				//		EmitError("Software error 4251354\n") 
				//	}
				//}else {
				//	if Down.GetType().GetType() != "point"
				//	{
					if UseDestructors
					{
						box2 := new FuncInputBox()  ; $temp
						box2.itPars.Emplace(Down.GetType(),Down.IsRef())
						func2 := FindFunc("~this",this&,box2^,true)
						if func2 != null
						{
							halfCall := new LinkForThis(Down,Down.GetType(),true)
							DestructFuncCall = MakeSimpleCall(func2,halfCall)
							DestructFuncCall.Line = Line
							DestructFuncCall.Up = this&
						}
					}
					//}
				//}
			}else{
				EmitError("can not delete item\n")
			}
		}
	}
	PrintInBlock := virtual !(TIOStream f) -> void
	{
		if DeleteFuncCall != null
		{
			Down.PrintPre(f)
			if DestructFuncCall != null
			{
				DestructFuncCall.PrintInBlock(f)
			}
			DeleteFuncCall.PrintInBlock(f)
		}
	}
}


ConstructCall := class extend NaturalCall
{
	this := !(BoxFunc^ func, Object^ Pars) -> void 
	{
		IsConstr = true
		Down = new LinkForThis(this&->{Object^},func.MyFuncType.Pars[0])
		Down.Right = Pars
		if Pars != null Pars.Left = Down

		RetId = GetNewId()
		ToCall = func
		FType = ToCall.MyFuncType
		Down.SetUp(this&)
		ExchangeParams()

		WorkBag.Push(this&,State_MiddleGetUse)

	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_MiddleGetUse
		{
			
			if Up?.GetValue() == "~Return()"
			{
				rights := Down.Right
				itm := GetItem(ReturnName,this&)
				parCall := new ParamNaturalCall("this",itm)
				parCall.Line = Line
				ReplaceNode(Down,parCall)
				TName = StrCopy("%"sbt + ReturnName)
			}else
			{
				//if not ToCall.IsRetRef wut???
				//{
					gotAlloc = true
					InAlloc = GetAlloc(this&,ToCall.MyFuncType.Pars[0])
					TName = StrCopy("%T"sbt + InAlloc)
					//TEName = "%TE" + RetId
				//}
			}
		}
	}
	FillConsts := !(TIOStream f) -> void
	{
		asNeed3 :=  ToCall.MyFuncType.Pars[0]
		if asNeed3 is TypeClass
		{
			asNeed2 := asNeed3->{TypeClass^}
			asNeed := asNeed2.ToClass

			asNeed.ApplyConstants(f,this&)
		}
	}
	GetType := virtual !() -> Type^
	{
		return ToCall.MyFuncType.Pars[0]
	}
	PrintPointPre := virtual !(TIOStream f) -> void
	{
		FillConsts(f)
		UseCall(f)
	}
	PrintPointUse := virtual !(TIOStream f) -> void
	{
		ToCall.MyFuncType.Pars[0].PrintType(f)
		f << "* " << TName
	}
	PrintInBlock := virtual !(TIOStream f) -> void
	{
		if ToCall.IsAssembler()
		{
			RealCall := ToCall->{BuiltInFunc^}
			RealCall.PrePrintEvent()
			PrintPreFuncName(f)
			RefsArr := FType.ParsIsRef

			if not RealCall.IsSelfPre
				PrintParamPres(f)
			
			AsmLine := RealCall.ToExe

			buf := char[2]
			buf[1] = 0

			thisName := GetName()
			j := 0
			while AsmLine[j] != 0
			{
				if AsmLine[j] != '#'
				{
					buf[0] = AsmLine[j]
					f << buf
					j += 1
				}else{
					j+= 1
					isFullName := false
					if AsmLine[j] == 'T'
					{
						isFullName = true
						j += 1
					}
					if AsmLine[j] in '0'..'9'
					{
						num := 0

						while AsmLine[j] in '0'..'9'
						{
							num *= 10
							num += AsmLine[j] - '0'
							j += 1
						}

						if num == 0
						{
							f << GetOutputName()
						}else{
							num -= 1
							miniIter := Down
							for num miniIter = miniIter.Right

							if miniIter == null
							{
								printf("nope %s\n",RealCall.FuncName)
							}

							if isFullName
							{
								if RefsArr[num] miniIter.PrintPointPre(f)
								else miniIter.PrintPre(f)
							}else{
								ToAdd := string
								if RefsArr[num] ToAdd = miniIter.GetPointName()
								else ToAdd = miniIter.GetName()

								f << ToAdd
							}
						}
					}
					if AsmLine[j] == 'd'
					{
						j += 1
						if DebugMode {
							newId := CreateDebugCall(this&)
							if newId != -1
							{
								f << ", !dbg !" << newId
							}
						}

					}
					if AsmLine[j] == '^'
					{
						numC := 0
						j += 1

						while AsmLine[j] in '0'..'9'
						{
							numC *= 10
							numC += AsmLine[j] - '0'
							j += 1
						}

						iterR := Down
						
						numC -= 1
						k := numC
						while k  != 0
						{
							iterR = iterR.Right
							k -= 1
						}

						if RealCall.MyFuncType.ParsIsRef[numC]
							iterR.PrintPointPre(f)
						else	iterR.PrintPre(f)

					}
					if AsmLine[j] == '#'
					{
						f << RetId
						j += 1
					}
				}
			}
		}else{

			FillConsts(f)
			UseCall(f)
		}
	}

	PrintPre := virtual !(TIOStream f) -> void
	{
		ErrorLog.Push("compiler error in func call, class asked by value")
	}
	PrintUse := virtual !(TIOStream f) -> void
	{
		//ErrorLog.Push("compiler error in func call, class asked by value")
		PrintPointUse(f) // жирный костыль
	}
	GetName := virtual !() -> string
	{
		ErrorLog.Push("compiler error in func call, class asked by value")
		return ""
	}
	GetPointName := virtual !() -> string
	{
		return TName
	}
}

LinkForThis := class extend Object
{
	Link := Object^
	ResultType := Type^
	keepClean := bool
	this := !(Object^ toCopy,Type^ tp,bool isKeep) -> void
	{
		Link = toCopy
		ResultType = tp
		keepClean = isKeep
	}
	this := !(Object^ toCopy,Type^ tp) -> void
	{
		Link = toCopy
		ResultType = tp
	}
	IsRef := virtual !() -> bool
	{
		return Link.IsRef()
	}
	GetValue := virtual !() -> char^
	{
		return Link.GetValue()
	}
	PrintGlobal := virtual !(TIOStream f) -> void
	{
	}
	PrintInBlock := virtual !(TIOStream f) -> void
	{
	}
	PrintPointPre := virtual !(TIOStream f) -> void
	{
		//Link.PrintPre(f)
	}
	PrintPointUse := virtual !(TIOStream f) -> void
	{
		if keepClean
		{
			Link.PrintPointUse(f)
		}else{
			Link.PrintUse(f)
		}
	}
	PrintPre := virtual !(TIOStream f) -> void
	{
		//if keepClean Link.PrintPre(f)
	}
	PrintUse := virtual !(TIOStream f) -> void
	{
		if keepClean Link.PrintUse(f)
	}
	GetName := virtual !() -> string
	{
		if keepClean
		{
			return Link.GetName()
		}
		return Link.GetPointName()
	}
	GetPointName := virtual !() -> string
	{
		return Link.GetPointName()
	}
	DoTheWork := virtual !(int st) -> void
	{
	}
	GetType := virtual !() -> Type^
	{
		return ResultType
	}
}

//PointCall
//MethodPointCall
//Operator()


