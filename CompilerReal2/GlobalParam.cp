GlobalParam := class extend MemParamCommon
{
	MainId := int
	IsThreadLocal := bool
	taskFieldId := int


	PrepareMainPtr := virtual !(TIOStream f,int newId) -> void {}
	GetMainPtr := virtual !(int newId) -> char^ { 
		return "@T"sbt + MainId
	}

	this := !(Type^ th,Object^ toSet) -> void
	{
		ResultType = th
		Down = toSet
		Down?.Up = this&
		MainId = GetNewId()
		WorkBag.Push(this&,State_PrePrint)
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_PrePrint
		{
			if Up? is ObjParam
			{
				CreateDbgGlobalVar(Up,ResultType,Up->{ObjParam^}.MyStr,false) //TODO: global ref?
			}
		}
	}

	PrintArrData := !(TIOStream f, Type^ toPr,bool isFirst) -> void
	{
		asArr := toPr->{TypeArr^}

		if not isFirst
			f << asArr.GetName() << " "

		bs := asArr.Base
		f << "["
		for i : asArr.Size
		{
			if i != 0
				f << " , "
			if bs is TypeClass
			{
				PrintClassData(f , bs->{TypeClass^}.ToClass,false)
			}else{
				assert(bs is TypeArr)
				PrintArrData(f, bs,false)
			}
		}
		f << "]\n"
	}
	PrintClassData := !(TIOStream f, BoxClass^ toPr, bool isFirst) -> void
	{
		if not isFirst
			f << toPr.ClassType.GetName() << " "
		addedVal := false
		f << "{"
		if toPr.Params.Size() == 0 and not toPr.ContainVirtual {
			f << "i8 0"
		}
		if toPr.ContainVirtual
		{
			addedVal = true
			f << "%ClassTableType" << toPr.ClassId << "* @ClassTableItem" << toPr.ClassId 
		}
		for itPr : toPr.Params
		{
			if addedVal 
				f << " , "
			else addedVal = true

			itCntV := TypeContainVTable(itPr.ResultType)
			if itPr.ResultType is TypeClass and itCntV
			{
				//f << itPr.ResultType.GetName() << " "
				PrintClassData(f,itPr.ResultType->{TypeClass^}.ToClass,false)
			}else{
				if itCntV
				{
					assert(itPr.ResultType is TypeArr)
					PrintArrData(f,itPr.ResultType,false)
				}else{
					f << itPr.ResultType.GetName() << " zeroinitializer"
				}
			}
		}
		f << "}\n"
	}
	PrintGlobal := virtual !(TIOStream f) -> void
	{
		if IsThreadLocal and jitMode
			return void
		
		f << "@T" << MainId << " = "//" = dso_local "
		if IsThreadLocal f << "thread_local "
		f << "global "
		if IsRef{
			ResultType.GetPoint().PrintType(f)
		}else{
			ResultType.PrintType(f)
		}
		if Down == null
		{
			if TypeContainVTable(ResultType)
			{
				if ResultType is TypeClass
				{
					PrintClassData(f,ResultType->{TypeClass^}.ToClass,true)
				}else{
					PrintArrData(f,ResultType,true)
				}
			}else{
				f << " zeroinitializer\n"
			}
		}else{
			f << " " << Down.GetName() << "\n"
		}
	}
}
