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

}
LocalParam := class extend MemParam
{
	inAllocId := int
	this := !(Type^ th,int allcId) -> void
	{
		ResultType = th
		inAllocId = allcId
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
		f << "%T" << newInd << " = load "
		ResultType.PrintType(f)
		f << " , "
		ResultType.PrintType(f)
		f << "* %T" << inAllocId << "\n"
	}
	PrintUse := virtual !(sfile f, int newInd) -> void
	{
		ResultType.PrintType(f)
		f << " %T"<< newInd
	}
	PrintPointPre := virtual !(sfile f, int newInd) -> void
	{
	}
	PrintPointUse := virtual !(sfile f, int newInd) -> void
	{
		ResultType.GetPoint().PrintType(f)
		f << " %T"<< inAllocId
	}
	GetName := virtual !(int newInd) -> string
	{
		Buf := char[256]
		sprintf(Buf,"%T%i",newInd)
		return Buf.Copy()
	}
	GetPointName := virtual !(int newInd) -> string
	{
		Buf := char[256]
		sprintf(Buf,"%T%i",inAllocId)
		return Buf.Copy()
	}
}
GlobalParam := class extend MemParam
{
	MainId := int
	this := !(Type^ th) -> void
	{
		ResultType = th
		MainId = GetNewId()
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
		f << "@T" << MainId << " = global "
		ResultType.PrintType(f)
		f << " zeroinitializer\n"
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
			f << "%" << ItName
		}
	}
	PrintUse := virtual !(sfile f, int newInd) -> void
	{
		ResultType.PrintType(f)
		f << " %" << ItName
	}
	GetPointName := virtual !(int newInd) -> string
	{
		//assert(IsRef)
		return "%" + ItName
	}
	GetName := virtual !(int newInd) -> string
	{
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
