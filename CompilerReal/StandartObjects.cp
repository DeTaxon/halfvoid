

ObjResult := class extend Object
{
	ResultType := Type^
	GetType := virtual !() -> Type^
	{
		return ResultType
	}
}

ObjConst := class extend ObjResult
{
	IsConst := virtual !() -> bool
	{
		return true
	}
	DoTheWork := virtual !(int pri) -> void
	{
		//noting is ok
	}
}

ObjInt := class extend ObjConst
{
	MyInt := s32
	"this" := !(int Value) -> void
	{
		MyInt = Value
		Clean()
		ResultType = GetType("int")
	}
	GetValue := virtual !() -> char^
	{
		return "~int"
	}
	Print := virtual !(int s) -> void
	{
		for s printf("->")
		printf("int %i\n",MyInt)
	}
	PrintUse := virtual !(sfile f) -> void
	{
		ResultType.PrintType(f)
		f << " " << MyInt
	}
	GetName := virtual !() -> string
	{
		buf := char[256]
		sprintf(buf,"%i",MyInt)
		return buf.Copy()
	}
	Clone := virtual !() -> Object^
	{
		PreRet := new ObjInt(MyInt)
		PreRet.Line = Line
		return PreRet
	}
}
ObjDouble := class extend ObjConst
{
	MyDouble := double
	"this" := !(double Value) -> void
	{
		MyDouble = Value
		Clean()
		ResultType = GetType("double")
	}
	GetValue := virtual !() -> char^
	{
		return "~double"
	}
	Print := virtual !(int s) -> void
	{
		for s printf("->")
		printf("double %f\n",MyDouble)
	}
	PrintUse := virtual !(sfile f) -> void
	{
		ResultType.PrintType(f)
		f << " " << MyDouble
	}
	GetType := virtual !() -> Type^
	{
		return GetType("double")
	}
	GetName := virtual !() -> string
	{
		buf := char[256]
		sprintf(buf,"%f",MyDouble)
		return buf.Copy()
	}
	Clone := virtual !() -> Object^
	{
		PreRet := new ObjDouble(MyDouble)
		PreRet.Line = Line
		return PreRet
	}
}

ObjStr := class extend ObjConst
{
	MyStrId := int
	MyTmpId := int
	"this" := !(char^ str) -> void
	{
		Clean()
		MyStrId = StrContainer.GetStringValue(str)
		MyTmpId = GetNewId()
		ResultType = GetType("string")
	}
	"this" := !(int SId) -> void
	{
		MyStrId = SId
		MyTmpId = GetNewId()
		ResultType = GetType("string")
	}
	GetString := virtual !() -> string
	{
		return StrContainer.GetString(MyStrId)
	}
	GetValue := virtual !() -> char^
	{
		return "~str"
	}
	Print := virtual !(int s) -> void
	{
		for s printf("->")
		printf("string %s\n",StrContainer.GetString(MyStrId))
	}
	PrintPre := virtual !(sfile f) -> void
	{
		StrSi := StrContainer.GetString(MyStrId).Size() + 1
		f << "%T" << MyTmpId <<" = getelementptr ["<< StrSi << " x i8] , [" << StrSi << " x i8]* @Str" << MyStrId <<", i32 0,i32 0\n" 
	}
	PrintUse := virtual !(sfile f) -> void
	{
		ResultType.PrintType(f)
		f << " %T" << MyTmpId	
	}
	Clone := virtual !() -> Object^
	{
		PreRet := new ObjStr(MyStrId)
		PreRet.Line = Line
		return PreRet
	}
}

ObjSymbol := class extend ObjConst
{
	MySymbol := char^
	"this" := !(char^ sym) -> void
	{
		MySymbol = sym
		Clean()
	}
	GetValue := virtual !() -> char^
	{
		return MySymbol
	}
	Print := virtual !(int s) -> void
	{
		for s printf("->")
		printf("symbol %s\n",MySymbol)
	}
	Clone := virtual !() -> Object^
	{
		PreRet := new ObjSymbol(MySymbol)
		PreRet.Line = Line
		return PreRet
	}
}


ObjCmd := class extend ObjConst
{
	MyStr := char^
	"this" := !(char^ st) -> void
	{
		Clean()
		MyStr = st
	}
	GetValue := virtual !() -> char^
	{
		return "~cmd"
	}
	Print := virtual !(int s) -> void
	{
		for s printf("->")
		printf("cmd %s\n",MyStr)
	}
	Clone := virtual !() -> Object^
	{
		PreRet := new ObjCmd(MyStr)
		PreRet.Line = Line
		return PreRet
	}
}
ObjKeyword := class extend ObjConst
{
	MyStr := char^
	"this" := !(char^ st) -> void
	{
		Clean()
		MyStr = st
	}
	GetValue := virtual !() -> char^
	{
		return MyStr
	}
	Print := virtual !(int s) -> void
	{
		for s printf("->")
		printf("keyword %s\n",MyStr)
	}
	Clone := virtual !() -> Object^
	{
		PreRet := new ObjKeyword(MyStr)
		PreRet.Line = Line
		return PreRet
	}
}
