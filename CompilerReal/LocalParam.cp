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
}
