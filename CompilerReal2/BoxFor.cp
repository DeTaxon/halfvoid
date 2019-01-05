#import "Tree.cp"

GetBoxFor := !(Object^ dat) -> BoxFor^
{

	itemName := string
	indName := string

	itemName = null

	iterY := dat.Down.Right

	Names := Queue.{string}()
	indNames := Queue.{string}()
	Downs := Queue.{Object^}()

	if iterY.Right.GetValue() == ","
	{
		itemName = iterY->{ObjIndent^}.MyStr
		indNames.Push(iterY.Right.Right->{ObjIndent^}.MyStr)
		
		iterY = iterY.Right.Right.Right.Right
	}else
	{
		indNames.Push(null->{string})
		if iterY.Right.GetValue() == ":"
		{
			itemName = iterY->{ObjIndent^}.MyStr

			iterY = iterY.Right.Right
		}
	}
	if iterY.Right.GetValue() == ","
	{
		Downs.Push(iterY)
		if itemName == null{
			Names.Push("it")
		}else{
			Names.Push(itemName)
		}

		iterY = iterY.Right
		while iterY.GetValue() == ","
		{
			iterY = iterY.Right
			asNeed := iterY->{ObjIndent^}
			Names.Push(asNeed.MyStr)
			iterY = iterY.Right.Right
			Downs.Push(iterY)
			indNames.Push(null->{string}) 
			iterY = iterY.Right
		}

		return new BoxForOldFashionMulti(Names,indNames,Downs,iterY)
	}
	Downs.Push(iterY)

	if itemName == null->{int^} {
		Names.Push("it")
	}else{
		Names.Push(itemName)
	}
	wut := iterY.Right

	return new BoxForOldFashionMulti(Names,indNames,Downs,wut)
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

	EnabledIIndex := bool

	this := !(Queue.{string} names, Queue.{string} f_ind,Queue.{Object^} items,Object^ itBlock) -> void
	{
		ContPath.Add(0)

		for items
		{
			mayb := TryCompute(it)
			if mayb != null
				it = mayb
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
		if pri == State_PreGetUse
		{
			WorkBag.Push(this&,State_GetUse)
			iter := Down.Right
			while iter != null
			{
				WorkBag.Push(iter,State_Start)
				iter = iter.Right
			}
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
					}else{
						someBox := new FuncInputBox()
						someBox.itPars.Emplace(Down.Right.GetType(),Down.Right.IsRef())

						func := FindFunc("~For",this&,someBox^,false)
						if func == null 
							EmitError("can not load ~For func\n")
						else{
							if func.MyFuncType.RetType == null
							{
								EmitError("can not deduce return type of function ~For\n")
							}else{

								if func.MyFuncType.RetType.GetType() != "class" EmitError("~For have to return class\n")
								else{
									ProxyFuncs[i] = func

									tmp := Down.Right
									PopOutNode(tmp)
									Downs.Push(MakeSimpleCall(func,tmp))
								}
							}
						}
					}
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
					iter2 = iter2.Next
					WorkBag.Push(iter,State_GetUse)
				}
				Down.SetUp(this&)
				IsStep1 = false
			}else{

				WorkBag.Push(Down,State_Start)
				iter := Down.Right
				for i : itemsCount
				{
					asNeed2 := iter->{SomeFuncCall^}
					itType := ProxyFuncs[i].MyFuncType.RetType
					ItId2 := asNeed2.GetItAllocId()

					ForItem := new LocalParam(itType,ItId2)
					
					asNeed := ((itType->{TypeClass^}).ToClass)

					emptyBox := new FuncInputBox()
					emptyBox.itPars.Emplace(itType,true)

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
					test = new ParamNaturalCall("",ForItem->{Object^})
					IncFuncs[i] = MakeSimpleCall(IncFuncP,test)

					if IsInvP != null
					{
						test = new ParamNaturalCall("",ForItem->{Object^})
						IsInvalids[i] = MakeSimpleCall(IsInvP,test)
					}

					if i == 0
					{
						IsEndFuncP := asNeed.GetFunc("IsEnd",emptyBox^,true)
						test = new ParamNaturalCall("",ForItem->{Object^})
						IsEndFunc = MakeSimpleCall(IsEndFuncP,test)
					}
					Params[i] = new RetFuncParam(UnrefFuncs[i])

					if IndNames[i] != null
					{
						itFunc4 := asNeed.GetFunc("Ind",emptyBox^,true)
						if itFunc4 == null
						{
							//EmitError("Can not get index item\n")
							EnabledIIndex = true
							IndParams[i] = new FuncParam("ForIndex" + ItId,GTypeInt,false)
						}else{
							test = new ParamNaturalCall("",ForItem->{Object^})
							IndFuncs[i] = MakeSimpleCall(itFunc4,test)
							IndParams[i] = new RetFuncParam(IndFuncs[i])
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
			if IndNames[i] != null
			{
				if IndNames[i] == name return IndParams[i]
			}
		}
		return null
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		iter := Down.Right
		while iter != null
		{
			iter.PrintPre(f)
			iter = iter.Right
		}

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

		if EnabledIIndex {
			f << "br label %ExtraStart" << ItId << "\n"
			f << "ExtraStart" << ItId << ":\n"
			f << "%ForIndex" << ItId << " = phi i32 [0,%start" << ItId << "] , [%FIndex" << ItId << ", %ExtraEnd" << ItId << "]\n"
		}
		IsEndFunc.PrintPre(f)
		f << "br i1 " << IsEndFunc.GetName() << " , label %End" << ItId << " , label %Next" << ItId << "\n"
		f << "Next" << ItId << ":\n"


		for i : itemsCount {
			if UnrefFuncs[i].IsRef() UnrefFuncs[i].PrintPointPre(f) else UnrefFuncs[i].PrintPre(f)
			if IndFuncs[i] != null
			{
				if IndFuncs[i].IsRef()
				{
					IndFuncs[i].PrintPointPre(f)
				}else{
					IndFuncs[i].PrintPre(f)
				}
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

		if UseRetPath
		{
			iter4 := Down.Right
			f << "br label %RetPath" << ItId << "\n"
			f << "RetPath" << ItId << ":\n"

			while iter4 != null
			{
				iter4.PrintDestructor(f)
				iter4 = iter4.Right
			}
			f << "br label %" << Up.GetOutPath(this&,PATH_RETURN,0) << "\n"
		}
		for itSize : ContPath
		{
			if itSize != 0{

				iter4 := Down.Right

				f << "br label %ContPath" << ItId << "id" << itSize <<"size\n"
				f << "ContPath" << ItId << "id" << itSize << "size:\n"

				while iter4 != null
				{
					iter4.PrintDestructor(f)
					iter4 = iter4.Right
				}
				
				f << "br label %" << Up.GetOutPath(this&,PATH_CONTINUE,itSize - 1) << "\n"
			}
		}
		for itSize : BreakPath
		{
			if itSize != 0{

				iter4 := Down.Right

				f << "br label %BreakPath" << ItId << "id" << itSize <<"size\n"
				f << "BreakPath" << ItId << "id" << itSize << "size:\n"

				while iter4 != null
				{
					iter4.PrintDestructor(f)
					iter4 = iter4.Right
				}
				
				f << "br label %" << Up.GetOutPath(this&,PATH_BREAK,itSize - 1) << "\n"
			}
		}
		f << "End" << ItId << ":\n"
	}

	UseRetPath := bool
	ContPath := Set.{int}
	BreakPath := Set.{int}

	GetOutPath := virtual !(Object^ itm, int typ, int size) -> string
	{
		if typ == PATH_RETURN
		{
			UseRetPath = true
			//if Up != null return Up.GetOutPath(this&,typ,size)
			return "RetPath" + ItId
		}
		if typ == PATH_CONTINUE
		{
			if size == 0{
				return "IncFuncs" + ItId
			}
			ContPath.Add(size)
			return "ContPath"sbt + ItId + "id" + size + "size"
		}
		if typ == PATH_BREAK
		{
			if size == 0{
				return "End" + ItId
			}
			BreakPath.Add(size)
			return "BreakPath"sbt + ItId + "id" + size + "size"
		}
		return ""
	}
	GetValue := virtual !() -> string
	{
		return "~~for()"
	}
}

