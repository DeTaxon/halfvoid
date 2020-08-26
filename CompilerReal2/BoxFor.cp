GetBoxFor := !(Object^ dat) -> BoxFor^
{

	itemName := string
	indName := string

	itemName = null

	iterY := dat.Down.Right

	Names := Queue.{string}() ; $temp
	indNames := Queue.{string}() ; $temp
	Downs := Queue.{Object^}() ; $temp
	if iterY.Right.Right == null
	{
		Names.Push("it")
		Downs.Push(iterY)
		indNames.Push("it_ind")
		iterY = iterY.Right
	}else{
		while iterY.Right != null
		{
			if iterY.Right.GetValue() == ","
			{
				Names.Push(iterY->{ObjIndent^}.MyStr)
				iterY = iterY.Right.Right
				indNames.Push(iterY->{ObjIndent^}.MyStr)
				iterY = iterY.Right.Right
			}else{ // ":"
				Names.Push(iterY->{ObjIndent^}.MyStr)
				iterY = iterY.Right.Right
				indNames.Push(null->{string})
			}
			Downs.Push(iterY)
			iterY = iterY.Right
			if iterY.GetValue() == ","
				iterY = iterY.Right

		}
	}
	preRes :=  new BoxForOldFashionMulti(Names,indNames,Downs,iterY)
	preRes.Line = dat.Down.Line

	if dat.Line?.itAttrs.Size() != 0
	{
		preRes.attrs = dat.Line.itAttrs&
	}
	return preRes
}

BoxFor := class extend Object
{
}

BoxForOldFashionMulti := class extend BoxFor
{
	ItId := int
	IsStep1 := bool

	itemsCount := int
	Names := string[]
	IndNames := string[]
	IncFuncs := Object^^
	UnrefFuncs := Object^^
	IndFuncs := Object^^
	IsInvalids := Object^^
	IsEndFunc := Object^

	ProxyFuncs := BoxFunc^^
	Params := MemParam^^
	IndParams := MemParam^^

	attrs := AttrArrayType^

	EnabledIIndex := bool
	CreatedIIndexNames := List.{char^}

	this := !(Queue.{string} names, Queue.{string} f_ind,Queue.{Object^} items,Object^ itBlock) -> void
	{
		for items
		{
			if it.IsConst 
				continue
			mayb := TryCompute(it)
			if mayb != null
				it = mayb.Clone()
		}

		Down = itBlock
		Down.Left = null
		itemsCount = items.Size()
		iter := Down

		for i : itemsCount, itm : items
		{
			iter.Right = itm
			iter.Right.Left = iter
			iter = iter.Right
		}
		iter.Right = null
		Down.SetUp(this&)
		MakeItBlock(Down)

		Names = names.ToArray()
		IndNames = new string[names.Size()]
		for it : names.Size(), itInd : f_ind
			IndNames[it] = itInd

		WorkBag.Push(this&,State_PreGetUse)

		IsStep1 = true

		ItId = GetNewId()
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if visitedWork[pri] return void
		else if pri != State_GetUse  visitedWork << pri

		if pri == State_Start
		{
			Down.Line = Line
			WorkBag.Push(this&,State_PreGetUse)
		}
		if pri == State_PreGetUse
		{
			while TryParseMacro(Down.Right,this&) != null {}
			WorkBag.Push(this&,State_GetUse)

			WorkBag.Push(Down.Right[^],State_Start)
		}
		if pri == State_GetUse
		{
			if IsStep1
			{
				IncFuncs = new Object^[itemsCount]
				UnrefFuncs = new Object^[itemsCount]
				IndFuncs = new Object^[itemsCount]
				ProxyFuncs = new BoxFunc^[itemsCount]
				Params = new MemParam^[itemsCount]
				IndParams = new MemParam^[itemsCount]
				IsInvalids = new Object^[itemsCount]

				for itemsCount IsInvalids[it] = null

				Downs:= Queue.{Object^}()
				for i : itemsCount
				{
					if Down.Right.GetType() == null
					{
						EmitError("can not evaluate type in for each\n")
						continue
					}

					someBox := new FuncInputBox() ; $temp
					someBox.itPars.Emplace(Down.Right.GetType(),Down.Right.IsRef())
					if attrs != null for q,k : attrs^
					{
						someBox.itAttrs[k] = q
					}

					func := FindFunc("~For",this&,someBox^,false)
					if func == null 
					{
						EmitError("can not load ~For func\n")
						continue
					}
					if func.MyFuncType.RetType == null
					{
						EmitError("can not deduce return type of function ~For\n")
						continue
					}

					if func.MyFuncType.RetType.GetType() != "class" {
						EmitError("~For have to return class\n")
						continue
					}

					ProxyFuncs[i] = func

					tmp := Down.Right
					PopOutNode(tmp)
					preSet := MakeSimpleCall(func,tmp)
					preSet.Line = Line
					preSet.Up = this&
					Downs.Push(preSet)
				}

				iter := Down
				iter2 := Downs.Start
				iter = Down
				WorkBag.Push(this&,State_GetUse)
				while iter2 != null
				{
					iter.Right = iter2.Data
					iter.Right.Left = iter
					iter = iter.Right
					iter2.Data.Up = this&
					iter2 = iter2.Next
					WorkBag.Push(iter,State_GetUse)
				}
				Down.SetUp(this&)
				IsStep1 = false
			}else{
				visitedWork << pri

				WorkBag.Push(Down,State_Start)
				iter := Down.Right
				for i : itemsCount
				{
					asNeed2 := iter->{SomeFuncCall^}
					itType := ProxyFuncs[i].MyFuncType.RetType
					ItId2 := asNeed2.GetItAllocId()

					ForItem := new LocalParam(itType,ItId2)
					
					asNeed := ((itType->{TypeClass^}).ToClass)

					emptyBox := new FuncInputBox() ; $temp
					emptyBox.itPars.Emplace(itType,true)
					if attrs != null for q,k : attrs^
					{
						emptyBox.itAttrs[k] = q
					}

					IncFuncP := asNeed.GetFunc("Inc",emptyBox^,true)
					UnrefFuncP := asNeed.GetFunc("^",emptyBox^,true)
					IsInvP := asNeed.GetFunc("IsInvalid",emptyBox^,true)

					if IncFuncP == null {
						EmitError("incorrect for iterator, need increment\n")
						return void
					}
					if UnrefFuncP == null {
						EmitError("incorrect for iterator, need ^\n")
						return void
					}


					test := new ParamNaturalCall("",ForItem->{Object^})
					UnrefFuncs[i] = MakeSimpleCall(UnrefFuncP,test)
					UnrefFuncs[i].Line = this.Line
					UnrefFuncs[i].Up = this&
					test = new ParamNaturalCall("",ForItem->{Object^})
					IncFuncs[i] = MakeSimpleCall(IncFuncP,test)
					IncFuncs[i].Line = this.Line
					IncFuncs[i].Up = this&

					if IsInvP != null
					{
						test = new ParamNaturalCall("",ForItem->{Object^})
						IsInvalids[i] = MakeSimpleCall(IsInvP,test)
						IsInvalids[i].Line = this.Line
						IsInvalids[i].Up = this&
					}

					if i == 0
					{
						IsEndFuncP := asNeed.GetFunc("IsEnd",emptyBox^,true)
						test = new ParamNaturalCall("",ForItem->{Object^})
						IsEndFunc = MakeSimpleCall(IsEndFuncP,test)
						IsEndFunc.Line = this.Line
						IsEndFunc.Up = this&
					}
					Params[i] = new RetFuncParam(UnrefFuncs[i])

					if IndNames[i] != null
					{
						itFunc4 := asNeed.GetFunc("Ind",emptyBox^,true)
						if itFunc4 == null
						{
							//EmitError("Can not get index item\n")
							EnabledIIndex = true
							IndParams[i] = new FuncParam("ForIndex"sbt + ItId,GTypeInt,false)
							if DebugMode
							{
								CreatedIIndexNames.Push(IndNames[i])
							}
						}else{
							test = new ParamNaturalCall("",ForItem->{Object^})
							IndFuncs[i] = MakeSimpleCall(itFunc4,test)
							IndParams[i] = new RetFuncParam(IndFuncs[i])
							IndFuncs[i].Line = this.Line
							IndFuncs[i].Up = this&
						}
					}
					iter = iter.Right
				}
			}
		}
	}
	GetItem := virtual !(string name) -> Object^
	{
		for i : itemsCount
		{
			if Names[i] == name 
			{
				return Params[i]
			}
			if IndNames[i]? == name
			{
				return IndParams[i]
			}
		}
		return null
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		if callDeferStuf
			PrintDeferDepth(f,ItId,this&)	
		Down.Right[^].PrintPre(f)

		debId := -1
		if DebugMode debId = CreateDebugCall(this&)

		Checks := 0
		for itemsCount if IsInvalids[it] != null Checks += 1

		if Checks > 0
		{
			for itemsCount
			{
				if IsInvalids[it] != null
				{
					
					IsInvalids[it].PrintPre(f)
					f << "br i1 " << IsInvalids[it].GetName() << ", label %End" << ItId
					if Checks > 1
					{
						f << ", label %NextCheck" << Checks << "\n"
						f << "NextCheck" << Checks << ":\n"
					}else{
						f << ", label %start" << ItId << "\n" // no need in standart for
						f << "start" << ItId << ":\n"
					}
				}
				Checks -= 1
			}
		}else{
			f << "br label %start" << ItId << "\n" // no need in standart for
			f << "start" << ItId << ":\n"
		}

		createdIInts := List.{int}() ; $temp

		if debId != -1 for it : CreatedIIndexNames
		{
			newRes := CreateDbgLocVar(Down,GTypeInt,it)
			if newRes != -1
				createdIInts.Push(newRes)
		}

		if EnabledIIndex {
			f << "br label %ExtraStart" << ItId << "\n"
			f << "ExtraStart" << ItId << ":\n"
			f << "%ForIndex" << ItId << " = phi i32 [0,%start" << ItId << "] , [%FIndex" << ItId << ", %ExtraEnd" << ItId << "]\n"

			for it : createdIInts
			{
				f << "call void @llvm.dbg.value(metadata i32 %ForIndex" << ItId << " ,metadata !" << it << ",metadata !DIExpression()), !dbg !"<<debId<<"\n" 
			}
		}
		IsEndFunc.PrintPre(f)
		f << "br i1 " << IsEndFunc.GetName() << " , label %End" << ItId << " , label %Next" << ItId << "\n"
		f << "Next" << ItId << ":\n"


		for i : itemsCount {
			if UnrefFuncs[i].IsRef() UnrefFuncs[i].PrintPointPre(f) else UnrefFuncs[i].PrintPre(f)
			if Params[i]? is RetFuncParam and Names[i] != null and debId != -1
			{
				Params[i]->{RetFuncParam^}.PrintForDebugDeclare(f,Names[i],Down,debId)
			}
			if IndFuncs[i] != null
			{
				if IndFuncs[i].IsRef()
				{
					IndFuncs[i].PrintPointPre(f)
				}else{
					IndFuncs[i].PrintPre(f)
				}
			}
			if IndFuncs[i] != null and IndParams[i]? is RetFuncParam and IndNames[i] != null and debId != -1
			{
				IndParams[i]->{RetFuncParam^}.PrintForDebugDeclare(f,IndNames[i],Down,debId)
			}
		}
		Down.PrintInBlock(f)
		
		f << "br label %IncFuncs" << ItId << "\n"
		f << "IncFuncs" << ItId << ":\n"
		for i : itemsCount IncFuncs[i].PrintPre(f)

		if EnabledIIndex
		{
			f << "br label %ExtraEnd" << ItId << "\n"
			f << "ExtraEnd" << ItId << ":\n"
			f << "%FIndex" << ItId << " = add i32 %ForIndex" << ItId << " , 1\n"
			f << "br label %ExtraStart" << ItId << "\n"
		}else{
			f << "br label %start" << ItId << "\n"
		}

		if useContinue
		{
			f << "PreContinue" << ItId << ":\n"
			if callDeferStuf
				PrintDeferApply(f,ItId,this&)
			f << "br label %IncFuncs" << ItId << "\n"
		}
		if useBreak
		{
			f << "PreEnd" << ItId << ":\n"
			if callDeferStuf
				PrintDeferApply(f,ItId,this&)
			f << "br label %End" << ItId << "\n"
		}
		f << "End" << ItId << ":\n"
	}

	useContinue := bool
	useBreak := bool
	callDeferStuf := bool
	ApplyDeferUse := virtual !(int depth) -> void
	{
		if depth != 1
		{
			Up.ApplyDeferUse(depth - 1)
		}else{
			callDeferStuf = true
		}
	}

	GetOutPath := virtual !(Object^ itm, int typ, int size) -> string
	{
		if typ == PATH_RETURN
		{
			return Up.GetOutPath(this&,typ,size) //poke
		}
		if typ == PATH_CONTINUE
		{
			if size == 0{
				useContinue = true
				return StrCopy("PreContinue"sbt + ItId)
			}
			return Up.GetOutPath(itm,typ,size - 1)
		}
		if typ == PATH_BREAK
		{
			if size == 0{
				useBreak = true
				return StrCopy("PreEnd"sbt + ItId)
			}
			return Up.GetOutPath(itm,typ,size - 1)
		}
		return ""
	}
	GetValue := virtual !() -> string
	{
		return "~~for()"
	}
}

