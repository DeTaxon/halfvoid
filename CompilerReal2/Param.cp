ObjParam := class extend Object
{
	MyStr := string
	ObjType := Type^
	Atter := Object^
	AskedGetUse := bool
	IsSetValue := bool
	IsFunc := bool
	IsStrName := bool
	IsExtern := bool
	IsTook := bool
	IsGlobal := bool
	IsRef := bool
	IsVirtual := bool
	IsThreadLocal := bool

	DestrCall := NaturalCall^
	
	this := !(string ss, bool IsstName) -> void
	{
		MyStr = StrCopy(ss)
		IsStrName = IsstName
	}
	GetValue := virtual !() -> string
	{
		return "i:=1"
	}
	GetType := virtual !() -> Type^
	{
		return ObjType
	}
	Print := virtual !(int lvl) -> void
	{
		for lvl printf("->")
		printf("param i:=1\n")
		iter := Down
		while iter != null
		{
			iter.Print(lvl+1)
			iter = iter.Right
		}
	}
	GetName := virtual !() -> string
	{
		return MyStr
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		if Down?.Right? is SLambda
		{
			asL := Down.Right->{SLambda^}
			asL.PrintPre(f)
			itMem := Down->{MemParam^}
			extrId := GetNewId()
			f << "store "
			asL.PrintUse(f)
			f << " , "
			itMem.PrintPointUse(f,extrId)
			f << "\n"

			return void
		}
		if ObjType is TypeClass and (not IsTook) and (not IsRef)
		{
			asNeed2 := ObjType->{TypeClass^}
			asNeed := asNeed2.ToClass
			
			if not IsGlobal
			{
				tmp := new ParamNaturalCall("_",Down)
				asNeed.ApplyConstants(f,tmp)
				//free(tmp)
			}
		}
		
		if IsTook
		{
			Down.Right.PrintInBlock(f)
		}else{
			if IsSetValue 
			{
				if IsRef
				{
					asLoc := Down->{LocalParam^}
					itType := Down.Right.GetType()
					Down.Right.PrintPointPre(f)
					f << "store " 
					Down.Right.PrintPointUse(f)
					f << " , " << itType.GetName() << "** %T" << asLoc.inAllocId << "\n"
					
				}else{
					asLoc := Down->{LocalParam^}
					Down.Right.PrintPre(f)
					f << "store "
					Down.Right.PrintUse(f)
					f << " , "
					asLoc.PrintPointUse(f,0)
					f << "\n"
				}
			}
		}
		if Down != null and Down is LocalParam and DebugMode
		{
			asLoc := Down->{LocalParam^}
			if IsRef
			{
			}else{
				outId := CreateDbgLocVar(this&,ObjType,MyStr)
				newId := CreateDebugCall(this&)
				if newId != -1 and outId != -1
				{
					f << "call void @llvm.dbg.declare(metadata " << ObjType.GetName() << "* %T" << asLoc.inAllocId << " , metadata !" << outId << " , metadata !DIExpression()) , !dbg !" << newId << "\n"
				}
			}
		}
	}
	PrintDestructor := virtual !(sfile f) -> void
	{
		if DestrCall != null and UseDestructors
		{
			DestrCall.RetId = GetNewId()
			asOb := DestrCall->{Object^}	
			asOb.PrintInBlock(f)
		}
	}
	//PrintGlobal := !(sfile f) -> void
	//{
	//	if not IsFunc or AskedGetUse
	//	{
	//		Down.PrintGlobal(f)
	//	}
	//}

	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			if Down?.GetValue() == "x=>x"
			{
				Down.DoTheWork(State_Start)
				asL := Down->{SLambda^}
				ObjType = asL.ResultType
				alId := GetAlloc(this&,ObjType)
				newT := MemParam^()
				if alId == -1
				{
					newT = new GlobalParam(asL.ResultType,null)
					newT.Down = Down
				}else{
					newT = new LocalParam(asL.ResultType,alId,false)
				}

				newT.Right= Down
				newT.Up = this&
				Down.Left = newT
				Down = newT
				asL.ApplyFunc()
				return void
			}
			if Line != null inhAttrs = Line.itAttrs&
			if Up != null and Up is BoxFile
			{
				asNeed := Up->{BoxFile^}
				asNeed.VisibleParams[MyStr].Push(this&) 
			}
			if IsVirtual {
				val := TryCompute(Down)
				if val != null{
					itr := this&->{Object^}
					while itr != null and itr.GetValue() != "{...}" itr = itr.Up
					if itr != null{
						asN := itr->{BoxClass^}
						asN.PutVirtualParam(MyStr,val)
						return void
					}
				}
			}
			asClass := ParseClass(Down)
			if asClass != null
			{
				Down = asClass
				asClass.Up = this&

				iterB := this&->{Object^}
				lazy := true

				while lazy
				{
					if iterB.GetValue() == "{d}" or iterB.GetValue() == "{d}.cp"
					{
						lazy = false
					}else{
						iterB = iterB.Up
						lazy = iterB != null
					}
				}
				if iterB != null
				{
					asNeed := iterB->{BoxBlock^}
					asNeed.Items.Push(new ObjHolder(MyStr,this&->{Object^}))
				}
			}else{
				WorkBag.Push(this&,State_CheckTypes)
				WorkBag.Push(this&,State_DestructCheck)

				iter := Down

				while iter != null
				{
					if iter.GetValue() == "()"
						WorkBag.Push(iter,State_Syntax)
					iter = iter.Right
				}
			}
		}

		if pri == State_CheckTypes
		{
			IsExtrn := false
			
			MaybeType := ParseType(Down)
			val := Object^
			val = null

			if MaybeType == null
			{
				val = TryCompute(Down)
				if val != null
				{
					MaybeType = val.GetType()
					Down = val
					Down.SetUp(this&)
				}
			}

			if MaybeType != null
			{
				allcId := GetAlloc(this&,MaybeType)
				ObjType = MaybeType
				if allcId == -1
				{
					//TODO: Global?
					if IsExtern
						Down = new ExternParam(MaybeType,val,MyStr)
					else{ 
						preRes := new GlobalParam(MaybeType,val)
						preRes.IsThreadLocal = IsThreadLocal
						Down = preRes 
					}
					IsGlobal = true

					Down.SetUp(this&)
				}else
				{
					if allcId == -2
					{
						asCl := GetUpClass(this&)
						if asCl != null
						{
							if Atter != null Down = new FakeFieldParam(MyStr,MaybeType,asCl,Atter)
							else Down = new FieldParam(MyStr,MaybeType,asCl)
							Down.SetUp(this&)
							Down.Line = Line
						}else{
							ErrorLog.Push("Compiler bag\n")
						}
					}else{
						if val != null
						{
							if IsRef EmitError("can not use ref on type\n")
							Down = new LocalParam(MaybeType,allcId)
							Down.inhAttrs = inhAttrs
							Down.Right = val
							val.Right = null
							val.Left = Down
							Down.SetUp(this&)
							IsSetValue = true
						}else{
							if IsRef EmitError("can not use ref on type\n")
							Down = new LocalParam(MaybeType,allcId)
							Down.inhAttrs = inhAttrs
							Down.SetUp(this&)
						}
					}
				}
			}else
			{
				SomeObj := ParseFuncDataR(Down)
				
				if SomeObj != null 
				{
					IsFunc = true
					Down = SomeObj
					SomeObj.SetUp(this&)
					ObjType = SomeObj.GetType()
					//WorkBag.Push(this&,State_GetUse)
				}else
				{
					asClass := ParseClass(Down)
					if asClass != null
					{
						Down = asClass
						asClass.Up = this&

						iterB := this&->{Object^}
						lazy := true

						while lazy
						{
							if iterB.GetValue() == "{d}" or iterB.GetValue() == "{d}.cp"
							{
								lazy = false
							}else{
								iterB = iterB.Up
								lazy = iterB != null
							}
						}
						if iterB != null
						{
							asNeed := iterB->{BoxBlock^}
							asNeed.Items.Push(new ObjHolder(MyStr,this&->{Object^}))
						}
					}else{
						WorkBag.Push(this&,State_PreGetUse)
					}
				}
			}
		}
		if pri == State_PreGetUse
		{
			if Down.GetValue() != "i:=2"
			{
				WorkBag.Push(Down,State_Start)
				WorkBag.Push(this&,State_GetUse)
			}
		}
		if pri == State_GetUse
		{
			if IsFunc 
			{
				AskedGetUse = true
			}else{
				if Down.GetValue() != "i:=2"
				{
					ObjType = Down.GetType()
					if ObjType != null
					{
						Temp := Down
						allcId := -1

						//injTest
						dwnIter := Down
						while dwnIter != null and dwnIter.GetValue() == "d()"
						{
							asNeed := dwnIter->{SomeFuncCall^}
							allcId = asNeed.GetItAllocId()
							if allcId != -1
							{
								IsTook = true
								break
							}else{
								if asNeed.ToCall?.IsSelfReturn
								{
									dwnIter = dwnIter.Down
								}else{
									break
								}
							}
						}

						//if Down.GetValue() == "d()"
						//{
						//	asNeed := Down->{SomeFuncCall^}
						//	allcId = asNeed.GetItAllocId()
						//	if allcId != -1 IsTook = true
						//}
						if allcId == -1 {
							if IsRef
								allcId = GetAlloc(this&,ObjType.GetPoint())
							else
								allcId = GetAlloc(this&,ObjType)
						}
						Down = new LocalParam(ObjType,allcId,IsRef)
						Down.Right = Temp
						Temp.Left = Down
						Down.SetUp(this&)
						IsSetValue = true
					}
				}
			}
		}
		if pri == State_DestructCheck
		{
			if ObjType != null and not IsRef
			{
				if ObjType.GetType() == "class"
				{	
					asClass := ObjType->{TypeClass^}

					box6 := new FuncInputBox() ; $temp

					box6.itPars.Emplace(asClass,true)
					asCl := asClass.ToClass
					DestructFunc := asCl.GetFunc("~this",box6^,true)
					if DestructFunc != null
					{
						asLoc := Down->{LocalParam^}
						parCall := new ParamNaturalCall("",asLoc->{Object^})
						DestrCall = MakeSimpleCall(DestructFunc,parCall)
					}
				}
			}
		}
	}
}
