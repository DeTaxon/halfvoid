MemParam := class extend ObjResult
{
	GetValue := virtual !() -> string
	{
		return "i:=2"
	}
	PrintPre := virtual !(sfile f, int newInd) -> void
	{
	}
	PrintUse := virtual !(sfile f, int newInd) -> void
	{
	}
	PrintPointPre := virtual !(sfile f, int newInd) -> void
	{
	}
	PrintPointUse := virtual !(sfile f, int newInd) -> void
	{
	}
	GetName := virtual !(int newInd) -> string
	{
		return ""
	}
	GetPointName := virtual !(int newInd) -> string
	{
		return ""
	}
	GetType := virtual !() -> Type^
	{
		return ResultType
	}
	GetSubName := virtual !() -> string
	{
		return "Mem"
	}

}
ConstParam := class extend MemParam
{
	itName := string
	this := !(string name, Type^ tp) -> void
	{
		itName = "%" + name
		ResultType = tp
	}
	PrintUse := virtual !(sfile f, int newInd) -> void
	{
		f << ResultType.GetName() <<" " <<itName
	}
	GetName := virtual !(int newInd) -> string
	{
		return itName
	}
	GetName := virtual !() -> string
	{
		return itName
	}
}
LocalParam := class extend MemParam
{
	inAllocId := int
	IsRef := bool
	this := !(Type^ th,int allcId) -> void
	{
		ResultType = th
		inAllocId = allcId
		IsRef = false
	}
	this := !(Type^ th,int allcId,bool asRef) -> void
	{
		ResultType = th
		inAllocId = allcId
		IsRef = asRef
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			//WorkBag.Push(this&,State_GetUse)
		}
		if pri == State_GetUse
		{
			//if inAllocId == -1 inAllocId = GetAlloc(this&,ResultType)
		}
	}
	PrintPre := virtual !(sfile f, int newInd) -> void
	{
		if IsRef {
			f << "%TPre" << newInd << " = load "
			ResultType.GetPoint().PrintType(f)
			f << " , "
			ResultType.GetPoint().PrintType(f)
			f << "* %T" << inAllocId << "\n"

			f << "%T" << newInd << " = load "
			ResultType.PrintType(f)
			f << " , "
			ResultType.PrintType(f)
			f << "* %TPre" << newInd << "\n"
			
		}else{
			f << "%T" << newInd << " = load "
			ResultType.PrintType(f)
			f << " , "
			ResultType.PrintType(f)
			f << "* %T" << inAllocId << "\n"
		}
	}
	PrintUse := virtual !(sfile f, int newInd) -> void
	{
		ResultType.PrintType(f)
		f << " %T"<< newInd
	}
	IsRef := virtual !() -> bool
	{
		return true
	}
	PrintPointPre := virtual !(sfile f, int newInd) -> void
	{
		if IsRef {
			f << "%T" << newInd << " = load "
			ResultType.GetPoint().PrintType(f)
			f << " , "
			ResultType.GetPoint().PrintType(f)
			f << "* %T" << inAllocId << "\n"
		}
	}
	PrintPointUse := virtual !(sfile f, int newInd) -> void
	{
		if IsRef{
			ResultType.GetPoint().PrintType(f)
			f <<" %T" << newInd
		}else{
			ResultType.GetPoint().PrintType(f)
			f << " %T"<< inAllocId
		}
	}
	GetName := virtual !(int newInd) -> string
	{
		Buf := char[256]
		sprintf(Buf,"%T%i",newInd)
		return Buf.Copy()
	}
	GetPointName := virtual !(int newInd) -> string
	{
		if IsRef{
			return "%T" + newInd
		}
		Buf := char[256]
		sprintf(Buf,"%T%i",inAllocId)
		return Buf.Copy()
	}
	GetSubName := virtual !() -> string
	{
		return "Local"
	}
}
VeryLocalParam := class extend LocalParam
{
	GetSubName := virtual !() -> string
	{
		return "VeryLocal"
	}
}
GlobalParam := class extend MemParam
{
	MainId := int
	IsThreadLocal := bool
	this := !(Type^ th,Object^ toSet) -> void
	{
		ResultType = th
		Down = toSet
		Down?.Up = this&
		MainId = GetNewId()
	}
	IsRef := virtual !() -> bool
	{
		return true
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			//WorkBag.Push(this&,State_GetUse)
		}
		if pri == State_GetUse
		{
			//if inAllocId == -1 inAllocId = GetAlloc(this&,ResultType)
		}
	}

	PrintArrData := !(sfile f, Type^ toPr) -> void
	{
		asArr := toPr->{TypeArr^}

		f << asArr.GetName() << " "
		bs := asArr.Base
		f << "["
		for i : asArr.Size
		{
			if i != 0
				f << " , "
			if bs is TypeClass
			{
				f << bs.GetName() << " "
				PrintClassData(f , bs->{TypeClass^}.ToClass)
			}else{
				assert(bs is TypeArr)
				PrintArrData(f, bs)
			}
		}
		f << "]"
	}
	PrintClassData := !(sfile f, BoxClass^ toPr) -> void
	{
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
				f << itPr.ResultType.GetName() << " "
				PrintClassData(f,itPr.ResultType->{TypeClass^}.ToClass)
			}else{
				if itCntV
				{
					assert(itPr.ResultType is TypeArr)
					PrintArrData(f,itPr.ResultType)
				}else{
					f << itPr.ResultType.GetName() << " zeroinitializer"
				}
			}
		}
		f << "}\n"
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
		f << "@T" << MainId << " = "//" = dso_local "
		if IsThreadLocal f << "thread_local "
		f << "global "
		ResultType.PrintType(f)
		if Down == null
		{
			if TypeContainVTable(ResultType)
			{
				PrintClassData(f,ResultType->{TypeClass^}.ToClass)
			}else{
				f << " zeroinitializer\n"
			}
		}else{
			f << " " << Down.GetName() << "\n"
		}
	}
	PrintPointPre := virtual !(sfile f, int newInd) -> void
	{
	}
	PrintPointUse := virtual !(sfile f, int newInd) -> void
	{
		ResultType.GetPoint().PrintType(f)
		f << " @T" << MainId
	}
	PrintPre := virtual !(sfile f, int newInd) -> void
	{
		f << "%T" << newInd << " = load "
		ResultType.PrintType(f)
		f << " , "
		ResultType.PrintType(f)
		f << "* @T" << MainId << "\n"
	}
	PrintUse := virtual !(sfile f, int newInd) -> void
	{
		ResultType.PrintType(f)
		f << " %T"<< newInd
	}
	GetName := virtual !(int newInd) -> string
	{
		return "%T" + newInd 
	}
	GetPointName := virtual !(int newInd) -> string
	{
		return "@T" + MainId
	}
}
GlobalFuncParam := class extend MemParam
{
	MainId := int
	this := !(Type^ th,Object^ toSet) -> void
	{
		ResultType = th
		Down = toSet
		MainId = GetNewId()
	}
	IsRef := virtual !() -> bool
	{
		return true
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			//WorkBag.Push(this&,State_GetUse)
		}
		if pri == State_GetUse
		{
			//if inAllocId == -1 inAllocId = GetAlloc(this&,ResultType)
		}
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
		f << "@T" << MainId << " = "
		f << "global "
		ResultType.PrintType(f)
		if Down == null
		{
			f << " zeroinitializer\n"
		}else{
			f << " " << Down.GetName() << "\n"
		}
	}
	PrintPointPre := virtual !(sfile f, int newInd) -> void
	{
	}
	PrintPointUse := virtual !(sfile f, int newInd) -> void
	{
		ResultType.GetPoint().PrintType(f)
		f << " @T" << MainId
	}
	PrintPre := virtual !(sfile f, int newInd) -> void
	{
		f << "%T" << newInd << " = load "
		ResultType.PrintType(f)
		f << " , "
		ResultType.PrintType(f)
		f << "* @T" << MainId << "\n"
	}
	PrintUse := virtual !(sfile f, int newInd) -> void
	{
		ResultType.PrintType(f)
		f << " %T"<< newInd
	}
	GetName := virtual !(int newInd) -> string
	{
		return "%T" + newInd 
	}
	GetPointName := virtual !(int newInd) -> string
	{
		return "@T" + MainId
	}
}
ExternParam := class extend MemParam
{
	MainId := int
	itName := string
	this := !(Type^ th,Object^ toSet,string itN) -> void
	{
		itName = itN
		ResultType = th
		Down = toSet
		MainId = GetNewId()
	}
	IsRef := virtual !() -> bool
	{
		return true
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			//WorkBag.Push(this&,State_GetUse)
		}
		if pri == State_GetUse
		{
			//if inAllocId == -1 inAllocId = GetAlloc(this&,ResultType)
		}
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
		f << "@" << itName << " = "
		f << "external global "
		ResultType.PrintType(f)
		f << "\n"
	}
	PrintPointPre := virtual !(sfile f, int newInd) -> void
	{
	}
	PrintPointUse := virtual !(sfile f, int newInd) -> void
	{
		ResultType.GetPoint().PrintType(f)
		f << " @" << itName
	}
	PrintPre := virtual !(sfile f, int newInd) -> void
	{
		f << "%T" << newInd << " = load "
		ResultType.PrintType(f)
		f << " , "
		ResultType.PrintType(f)
		f << "* @" << itName << "\n"
	}
	PrintUse := virtual !(sfile f, int newInd) -> void
	{
		ResultType.PrintType(f)
		f << " %T"<< newInd
	}
	GetName := virtual !(int newInd) -> string
	{
		return "%T" + newInd 
	}
	GetPointName := virtual !(int newInd) -> string
	{
		return "@" + itName
	}
}

FuncParam := class extend MemParam
{
	ItName := string
	IsRef := bool
	this := !(string Name,Type^ typ, bool IIsRef) -> void
	{
		ResultType = typ
		ItName = Name
		IsRef = IIsRef
	}
	PrintPre := virtual !(sfile f, int newInd) -> void
	{
		if IsRef
		{
			f << "%Tpre" << newInd << " = load "
			ResultType.PrintType(f)
			f << " , "
			ResultType.PrintType(f)
			f << "* %" << ItName << "\n"
		}
	}
	PrintPointPre := virtual !(sfile f, int newInd) -> void
	{
	}
	PrintPointUse := virtual !(sfile f, int newInd) -> void
	{
		
		if IsRef
		{
			ResultType.GetPoint().PrintType(f)
			f << " %" << ItName
		}
	}
	PrintUse := virtual !(sfile f, int newInd) -> void
	{
		if IsRef
		{
			ResultType.PrintType(f)
			f << " %Tpre" << newInd
		}else{
			ResultType.PrintType(f)
			f << " %" << ItName
		}
	}
	GetPointName := virtual !(int newInd) -> string
	{
		//assert(IsRef)
		return "%" + ItName
	}
	GetName := virtual !(int newInd) -> string
	{
		if IsRef
		{
			return "%Tpre" + newInd
		}
		return "%" + ItName
	}
	
}

FieldParam := class extend MemParam
{
	ItName := string
	ToMerge := BoxClass^

	this := !(string Name,Type^ typ, BoxClass^ pp) -> void
	{
		ResultType = typ
		ItName = Name
		ToMerge = pp
		pp.Params.Push(this&)
	}
	IsRef := virtual !() -> bool
	{
		return true
	}
	PrintUse := virtual !(sfile f, int newInd) -> void
	{
		ResultType.PrintType(f)
		f << " %" << ItName
	}
	GetName := virtual !(int newInd) -> string
	{
		return "%" + ItName
	}
}
FakeFieldParam := class extend MemParam
{
	ItName := string
	ToMerge := BoxClass^
	Atter := Object^
	this := !(string Name,Type^ typ, BoxClass^ pp,Object^ att) -> void
	{
		ResultType = typ
		ItName = Name
		ToMerge = pp
		Atter = att
		pp.FakeParams.Push(this&)
	}
	IsRef := virtual !() -> bool
	{
		return true
	}
	PrintUse := virtual !(sfile f, int newInd) -> void
	{
		ResultType.PrintType(f)
		f << " %" << ItName
	}
	GetName := virtual !(int newInd) -> string
	{
		return "%" + ItName
	}
}
RetFuncParam := class extend MemParam
{
	ToCall := Object^
	this := !(Object^ func) -> void
	{
		ToCall = func
		ResultType = func.GetType()
	}
	IsRef := virtual !() -> bool
	{
		return ToCall.IsRef()
	}
	PrintPre := virtual !(sfile f, int newInd) -> void
	{
		//ToCall.PrintPre(f)
		if ToCall.IsRef()
		{
			f << "%T" << newInd << " = load " << ResultType.GetName() << " , " 
			ToCall.PrintPointUse(f)
			f << "\n"
		}
	}
	PrintUse := virtual !(sfile f, int newInd) -> void
	{
		if ToCall.IsRef()
		{
			f << ResultType.GetName() << " %T" << newInd << "\n"
		}else{
			ToCall.PrintUse(f)
		}
	}
	PrintPointPre := virtual !(sfile f, int newInd) -> void
	{
		//ToCall.PrintPointPre(f)
	}
	PrintPointUse := virtual !(sfile f, int newInd) -> void
	{
		ToCall.PrintPointUse(f)
	}
	GetName := virtual !(int newInd) -> string
	{
		if ToCall.IsRef()
		{
			return "%T" + newInd
		}
		return ToCall.GetName()
	}
	GetType := virtual !() -> Type^ { 
		return ResultType 
	}
	GetPointName := virtual !(int newInd) -> string
	{
		return ToCall.GetPointName()
	}
}
