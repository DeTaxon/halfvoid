
NewCallOne := class extend SomeFuncCall
{
	newItm := BaseFuncCall^
	newType := Type^
	ItId := int
	useConstr := bool
	appendTemp := bool

	gcAllId := int
	this := !(Type^ nT,Object^ DW) -> void
	{
		useConstr = DW != null
		if DW != null	Down = DW.Down
		if Down != null
		{
			Down.Up.Down = null
			Down.SetUp(this&)
		}
		WorkBag.Push(this&,State_GetUse)

		ItId = GetNewId()
		newType = nT	
		ResultType = nT.GetPoint()

		if Down != null TrimCommas(this&->{Object^})
	}
	IsRef := virtual !() -> bool
	{
		return false
	}
	GetType := virtual !() -> Type^
	{
		return ResultType
	}
	GetName := virtual !() -> string
	{
		//return newItm.GetName()
		return "%Pre"sbt + ItId
	}
	GetPointName := virtual !() -> string
	{
		return "%Pre"sbt + ItId
	}
	PrintUse := virtual !(TIOStream f) -> void
	{
		//newItm.PrintUse(f)
		f << ResultType.GetName() << " %Pre" << ItId
	}
	PrintPre := virtual !(TIOStream f) -> void
	{
		newItm.PrintInBlock(f)
		f << "%Pre" << ItId << " = bitcast "
		newItm.PrintUse(f)
		f << " to " << ResultType.GetName() << "\n"

		if newType.GetType() == "class"
		{
			asNeed := newType->{TypeClass^}
			if asNeed.ToClass.IsGC
			{
				f << "call void@" << gcIncRefFunc.OutputName << "("
				newItm.PrintUse(f)
				f << ")"
				if DebugMode 
				{
					dId := CreateDebugCall(this&)
					f << ", !dbg !" << dId
				}
				f << "\n"
				f << "store " << newType.GetName() << "* %Pre" << ItId << " , "
				f << newType.GetName() << "** %T" << gcAllId << "\n"
			}
			asNeed.ToClass.ApplyConstants(f,this&)
		}

		if Down != null Down.PrintInBlock(f)

	}
	PrintInBlock := virtual !(TIOStream f) -> void
	{
		PrintPre(f)
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_GetUse	
		{
			box := new FuncInputBox()  ; $temp
			box.itConsts.Push(new ObjType(newType)) 
			FillAttrs(box^,this&)

			if Line != null {
				for v,k : Line.itAttrs box.itAttrs[k] = v
			}

			if appendTemp
			{
				box.itAttrs["$temp"] = new ObjBool(true) //GBoolTrue
			}

			func := BoxFunc^
			func = null

			if newType is TypeClass
			{
				asTPre := newType->{TypeClass^}
				asT := asTPre.ToClass
				func = asT.GetFunc("new",box^,true)
			}

			if func == null{
				func = FindFunc("new",this&,box^,false)
			}

			if func == null 
			{
				EmitError("cant create type\n")
			}else{
				func.Line = Line
				newItm = MakeSimpleCall(func,null->{Object^})
				newItm.Line = Line
				newItm.Up = this&

				if useConstr
				{
					if newType is TypeClass
					{
						box := new FuncInputBox()  ; $temp

						box.itPars.Emplace(newType,true)
						iter3 := Down

						while iter3 != null
						{
							box.itPars.Emplace(iter3.GetType(),iter3.IsRef())
							iter3 = iter3.Right
						}

						asNeed := newType->{TypeClass^}


						constrFunc := asNeed.ToClass.GetFunc("this",box^,false)

						if constrFunc == null 
						{
							EmitError("can not find constructor\n")
						}else{
							extraF := new LinkForThis(this&->{Object^},newType)
							extraF.Right = Down
							extraF.Up = this&
							if Down != null Down.Left = extraF
							Down = extraF

							Down = MakeSimpleCall(constrFunc,Down)
							Down.Line = Line
							Down.Up = this&
						}
					}
				}
				if IsGCClass(newType)
				{
					gcAllId = GetAlloc(this&,newType.GetPoint())
					assert(gcAllId > 0)
					GCMakeAware(this&,gcAllId)
				}
			}
		}
	}
}

NewCall := class extend SomeFuncCall
{
	ExtraFunc := BaseFuncCall^
	Constr := BaseFuncCall^
	ConstrPars := Object^

	this := !(Type^ toCreate) -> void
	{
		This2(toCreate,new ObjInt(1))
	}
	this := !(Type^ toCreate,Object^ toCr) -> void
	{
		if toCr.GetValue() == "()"
		{
			This2(toCreate,new ObjInt(1))
			ConstrPars = toCr.Down

			if ConstrPars != null
				ConstrPars.SetUp(null->{Object^})

			
			if toCreate is TypeClass
			{
				b := new FuncInputBox()  ; $temp
				b.itPars.Emplace(toCreate,true)
				FillAttrs(b^,toCr)

				iter := ConstrPars
				while iter != null
				{
					b.itPars.Emplace(iter.GetType(),iter.IsRef())
					iter = iter.Right
				}
				asClass := toCreate->{TypeClass^}
				func := asClass.ToClass.GetFunc("this",b^,true)
				if func != null
				{
					Temp := ConstrPars
					ConstrPars = new LinkForThis(this&->{Object^},toCreate->{Type^})
					if Temp != null
					{
						ConstrPars.Right = Temp
						Temp.Left = ConstrPars
					}
					Constr = MakeSimpleCall(func,ConstrPars)
				}else{
					//ErrorLog.Push("constructor no found at " + Line.LinePos + "in file " +  Line.inFile)
					ErrorLog.Push("constructor no found")
				}

			}else{
			}
		}else{
			This2(toCreate,toCr)
		}
	}
	This2 := !(Type^ toCreate,Object^ toCr) -> void
	{
		oldUp := toCr.Up
		box := new FuncInputBox()  ; $temp
		//ResultType = toCreate.GetPoint()
		//Down = new TypeSizeCall(toCreate)
		Down = toCr
		Down.SetUp(this&)

		box.itPars.Emplace(GTypeInt,false)
		box.itConsts.Push(new ObjType(toCreate))
		FillAttrs(box^,oldUp)

		fun := FindFunc("new",this&,box^,false)
		if fun == null
		{
			toCr.EmitError("can not create new obj\n")
			return void
		}
		ResultType = fun.MyFuncType.RetType
		ExtraFunc = MakeSimpleCall(fun,Down)
		ExtraFunc.Down.SetUp(ExtraFunc)
		CheckReturn()
	}
	UseCall := virtual !(TIOStream f) -> void
	{
		ExtraFunc.PrintInBlock(f)
		if ResultType.Base is TypeClass
		{
			asNeed := (((ResultType.Base)->{TypeClass^}).ToClass)
			asNeed.ApplyConstants(f,ExtraFunc)
		}
		if Constr != null
			Constr.PrintInBlock(f)
	}
	GetType := virtual !() -> Type^
	{
		return ResultType
	}
	GetName := virtual !() -> string
	{
		return ExtraFunc.GetName()
	}
	GetPointName := virtual !() -> string
	{
		return ""
	}
	PrintUse := virtual !(TIOStream f) -> void
	{
		ExtraFunc.PrintUse(f)
	}
}
