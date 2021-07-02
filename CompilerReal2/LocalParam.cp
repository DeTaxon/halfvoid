MemParam := class extend ObjResult
{
	GetValue := virtual !() -> string
	{
		return "i:=2"
	}
	PrintPre := virtual !(TIOStream f, int newInd,int debId) -> void
	{
	}
	PrintUse := virtual !(TIOStream f, int newInd,int debId) -> void
	{
	}
	PrintPointPre := virtual !(TIOStream f, int newInd,int debId) -> void
	{
	}
	PrintPointUse := virtual !(TIOStream f, int newInd,int debId) -> void
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
	PrintDebugDeclare := virtual !(TIOStream f,Object^ to,char^ forceName) -> void
	{
	}
	PrepareMainPtr := virtual !(TIOStream f,int newId,int debId) -> void {}
	GetMainPtr := virtual !(int newId) -> char^ { return "wow"}
}

IsMemParam := !(Object^ obj) -> bool
{
	if obj is LocalParam return true
	if obj is GlobalParam return true
	return false
}
MemParamCommon := class extend MemParam
{
	IsRef := bool

	IsRef := virtual !() -> bool //IsRValue
	{
		return true //IsRef func and IsRef bool is different 
	}
	PrintPointPre := virtual !(TIOStream f, int newInd,int debId) -> void
	{
		PrepareMainPtr(f,newInd,debId)
		if IsRef {
			f << "%TRefP" << newInd << " = load "
			ResultType.GetPoint().PrintType(f)
			f << " , "
			ResultType.GetPoint().GetPoint().PrintType(f)
			f << " "<< GetMainPtr(newInd) << "\n"
		}
	}
	PrintPointUse := virtual !(TIOStream f, int newInd,int debId) -> void
	{
		if IsRef {
			ResultType.GetPoint().PrintType(f)
			f << " %TRefP" << newInd
			return void
		}
		ResultType.GetPoint().PrintType(f)
		f << " " << GetMainPtr(newInd)
	}
	GetPointName := virtual !(int newInd) -> char^
	{	
		if IsRef {
			return "%TRefP"sbt + newInd
		}
		return GetMainPtr(newInd)
	}
	PrintPre := virtual !(TIOStream f, int newInd,int debId) -> void
	{
		PrintPointPre(f,newInd,debId)
		f << "%TRes" << newInd << " = load "
		ResultType.PrintType(f)
		f << " , "
		PrintPointUse(f,newInd,debId) 
		if DebugMode and debId != -1
			f << ", !dbg !" << debId 
		f << "\n"
	}
	PrintUse := virtual !(TIOStream f, int newInd,int debId) -> void
	{
		ResultType.PrintType(f)
		f << " %TRes"<< newInd
	}
	GetName := virtual !(int newInd) -> char^
	{
		return "%TRes"sbt + newInd 
	}
	GetRefPointName := virtual !(int newInd) -> char^
	{
		return GetMainPtr(newInd)
	}
}
GlobalParam := class extend MemParamCommon
{
	MainId := int
	IsThreadLocal := bool
	taskFieldId := int


	PrepareMainPtr := virtual !(TIOStream f,int newId) -> void {}
	GetMainPtr := virtual !(int newId) -> char^ { return "@T"sbt + MainId}

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
LocalParam := class extend MemParamCommon
{
	inAllocId := int
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

	PrepareMainPtr := virtual !(TIOStream f,int newId,int debId) -> void {}
	GetMainPtr := virtual !(int newId) -> char^ { return "%T"sbt + inAllocId}

	PrintDebugDeclare := virtual !(TIOStream f,Object^ fnc,char^ forceName) -> void
	{
		itDbgName := forceName
		if itDbgName == null
		{
			asP := Up->{ObjParam^}
			if Up == null or not (Up is ObjParam)
				return void
			itDbgName = asP.MyStr
		}
		//if IsRef
		//{
		//}else{
			asUp := Up
			if fnc != null asUp = fnc
			outId := CreateDbgLocVar(asUp,ResultType,itDbgName,IsRef)
			newId := CreateDebugCall(asUp)
			if newId != -1 and outId != -1
			{
				f << "call void @llvm.dbg.declare(metadata " << ResultType.GetName() << "*"
				if IsRef f << "*"
				f << " %T" << inAllocId << " , metadata !" << outId << " , metadata !DIExpression()) , !dbg !" << newId << "\n"
			}
		//}
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
	PrintGlobal := virtual !(TIOStream f) -> void
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
	PrintPointPre := virtual !(TIOStream f, int newInd,int debId) -> void
	{
	}
	PrintPointUse := virtual !(TIOStream f, int newInd,int debId) -> void
	{
		ResultType.GetPoint().PrintType(f)
		f << " @T" << MainId
	}
	PrintPre := virtual !(TIOStream f, int newInd,int debId) -> void
	{
		f << "%T" << newInd << " = load "
		ResultType.PrintType(f)
		f << " , "
		ResultType.PrintType(f)
		f << "* @T" << MainId
		if DebugMode and debId != -1
			f << ", !dbg !" << debId 
		f << "\n"
	}
	PrintUse := virtual !(TIOStream f, int newInd,int debId) -> void
	{
		ResultType.PrintType(f)
		f << " %T"<< newInd
	}
	GetName := virtual !(int newInd) -> string
	{
		return "%T"sbt + newInd 
	}
	GetPointName := virtual !(int newInd) -> string
	{
		return "@T"sbt + MainId
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
	PrintGlobal := virtual !(TIOStream f) -> void
	{
		f << "@" << itName << " = "
		f << "external global "
		ResultType.PrintType(f)
		f << "\n"
	}
	PrintPointPre := virtual !(TIOStream f, int newInd,int debId) -> void
	{
	}
	PrintPointUse := virtual !(TIOStream f, int newInd,int debId) -> void
	{
		ResultType.GetPoint().PrintType(f)
		f << " @" << itName
	}
	PrintPre := virtual !(TIOStream f, int newInd,int debId) -> void
	{
		f << "%T" << newInd << " = load "
		ResultType.PrintType(f)
		f << " , "
		ResultType.PrintType(f)
		f << "* @" << itName 
		if DebugMode and debId != -1
			f << ", !dbg !" << debId 
		f << "\n"
	}
	PrintUse := virtual !(TIOStream f, int newInd,int debId) -> void
	{
		ResultType.PrintType(f)
		f << " %T"<< newInd
	}
	GetName := virtual !(int newInd) -> string
	{
		return "%T"sbt + newInd 
	}
	GetPointName := virtual !(int newInd) -> string
	{
		return "@"sbt + itName
	}
}

FuncParam := class extend MemParam
{
	ItName := string
	IsRef := bool
	this := !(string Name,Type^ typ, bool IIsRef) -> void
	{
		ResultType = typ
		ItName = StrCopy(Name)
		IsRef = IIsRef
	}
	PrintPre := virtual !(TIOStream f, int newInd,int debId) -> void
	{
		if IsRef
		{
			f << "%Tpre" << newInd << " = load "
			ResultType.PrintType(f)
			f << " , "
			ResultType.PrintType(f)
			f << "* %" << ItName
			if DebugMode and debId != -1
				f << ", !dbg !" << debId 
			f << "\n"
		}
	}
	PrintPointPre := virtual !(TIOStream f, int newInd,int debId) -> void
	{
	}
	PrintPointUse := virtual !(TIOStream f, int newInd,int debId) -> void
	{
		
		if IsRef
		{
			ResultType.GetPoint().PrintType(f)
			f << " %" << ItName
		}
	}
	PrintUse := virtual !(TIOStream f, int newInd,int debId) -> void
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
		return "%"sbt + ItName
	}
	GetName := virtual !(int newInd) -> string
	{
		if IsRef
		{
			return "%Tpre"sbt + newInd
		}
		return "%"sbt + ItName
	}
	
}

FieldParam := class extend MemParam
{
	ItName := string
	ToMerge := BoxClass^

	this := !(string Name,Type^ typ, BoxClass^ pp) -> void
	{
		ResultType = typ
		ItName = StrCopy(Name)
		ToMerge = pp
		pp.Params.Push(this&)
	}
	IsRef := virtual !() -> bool
	{
		return true
	}
	PrintUse := virtual !(TIOStream f, int newInd,int debId) -> void
	{
		ResultType.PrintType(f)
		f << " %" << ItName
	}
	GetName := virtual !(int newInd) -> string
	{
		return "%"sbt + ItName
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
	PrintUse := virtual !(TIOStream f, int newInd) -> void
	{
		ResultType.PrintType(f)
		f << " %" << ItName
	}
	GetName := virtual !(int newInd) -> string
	{
		return "%"sbt + ItName
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
	PrintPre := virtual !(TIOStream f, int newInd,int debId) -> void
	{
		//ToCall.PrintPre(f)
		if ToCall.IsRef()
		{
			f << "%T" << newInd << " = load " << ResultType.GetName() << " , " 
			ToCall.PrintPointUse(f)
			if DebugMode and debId != -1
				f << ", !dbg !" << debId 
			f << "\n"
		}
	}
	PrintUse := virtual !(TIOStream f, int newInd,int debId) -> void
	{
		if ToCall.IsRef()
		{
			f << ResultType.GetName() << " %T" << newInd << "\n"
		}else{
			ToCall.PrintUse(f)
		}
	}
	PrintPointPre := virtual !(TIOStream f, int newInd,int debId) -> void
	{
		//ToCall.PrintPointPre(f)
	}
	PrintPointUse := virtual !(TIOStream f, int newInd,int debId) -> void
	{
		ToCall.PrintPointUse(f)
	}
	GetName := virtual !(int newInd) -> string
	{
		if ToCall.IsRef()
		{
			return "%T"sbt + newInd
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
	PrintForDebugDeclare := virtual !(TIOStream f,char^ forceName,Object^ ptrPos,int debId) -> void
	{
		if debId == -1 return void

		outId := CreateDbgLocVar(ptrPos,ResultType,forceName,IsRef())
		if debId != -1 and outId != -1
		{
			f << "call void @llvm.dbg."
			if IsRef() f << "addr" else f << "value"
			f << "(metadata "
			if IsRef() PrintPointUse(f,0,-1) else PrintUse(f,0,-1)
			f << " , metadata !" << outId << " , metadata !DIExpression()) , !dbg !" << debId << "\n"
		}
	}
}
