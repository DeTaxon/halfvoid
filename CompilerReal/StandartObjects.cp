

ObjResult := class extend Object
{
	ResultType := Type^
	GetType := virtual !() -> Type^
	{
		return ResultType
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
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

ObjHolder := class extend Object
{
	ItName := string
	this := !(string name,Object^ ob) -> void
	{
		ItName = name
		Down = ob
	}
	GetValue := virtual !() -> string
	{
		return "{object}"
	}
}

ObjConstHolder := class extend Object
{
	ItName := string

	this := !(string name,Object^ cons) -> void
	{
		ItName = name
		Down = cons.Clone()
		Down.Up = Down
	}

	GetValue := virtual !() -> string
	{
		return "{const}"
	}
}

ObjNULL := class extend ObjConst
{
	"this" := !() -> void
	{
		ResultType = VoidPType 
	}
	GetValue := virtual !() -> char^
	{
		return "~null"
	}
	Print := virtual !(int s) -> void
	{
		for s printf("->")
		printf("null\n")
	}
	PrintUse := virtual !(sfile f) -> void
	{
		f << "i8* null"
	}
	GetName := virtual !() -> string
	{
		return "null"
	}
	Clone := virtual !() -> Object^
	{
		PreRet := new ObjNULL()
		PreRet.Line = Line
		return PreRet
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
ObjBool := class extend ObjConst
{
	MyBool := bool
	this := !(bool Value) -> void
	{
		MyBool = Value
		ResultType = GetType("bool")
	}
	GetValue := virtual !() -> char^
	{
		return "~bool"
	}
	Print := virtual !(int s) -> void
	{
		for s printf("->")
		printf("bool %i\n",MyBool)
	}
	PrintUse := virtual !(sfile f) -> void
	{
		ResultType.PrintType(f)
		f << " "
		if MyBool f << "1" else f << "0"
	}
	GetName := virtual !() -> string
	{
		if MyBool return "1"
		return "0"
	}
	Clone := virtual !() -> Object^
	{
		PreRet := new ObjBool(MyBool)
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
	this := !(char^ str) -> void
	{
		Clean()
		MyStrId = StrContainer.GetStringValue(str)
		MyTmpId = GetNewId()
		ResultType = GetType("string")
	}
	this := !(int SId) -> void
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
	GetName := virtual !() -> string
	{
		return "%T" + MyTmpId
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
ObjArray := class extend ObjConst
{
	itType := TypeArr^
	Items := Object^^
	MyTmpId := int

	this := !(TypeArr^ ToUs,Object^^ ims) -> void
	{
		MyTmpId = GetNewId()
		itType = ToUs
		Items = ims
		ResultType = itType.Base.GetPoint()
	}		
	

	PrintGlobal := virtual !(sfile f) -> void
	{
		asBase := itType->{Type^}
		for itType.Size Items[it].PrintGlobal(f)
		f << "@Arr" << MyTmpId << " = global " << asBase.GetName() << " ["
		for i : itType.Size 
		{
			if i > 0 f << " , "
			f << Items[i].GetType().GetName() << " " << Items[i].GetName()
		}
		f << "]"
	}
	PrintPre := virtual !(sfile f) -> void
	{
		asBase := itType->{Type^}
		f << "%ArrTmp" << MyTmpId << " = getelementptr " << asBase.GetName() << " , " << asBase.GetName() << "* @Arr" << MyTmpId << " , i32 0, i32 0\n"	
	}
	PrintUse := virtual !(sfile f) -> void
	{
		ResultType.PrintType(f)
		f << " %ArrTmp"	<< MyTmpId
	}
	GetName := virtual !() -> string
	{
		return "null"
	}
	GetValue := virtual !() -> string
	{
		return "~![]"
	}
	Clone := virtual !() -> Object^
	{
		itItems := new Object^[itType.Size]
		for i : itType.Size itItems[i] = Items[i].Clone()
		PreRet := new ObjArray(itType,Items)
		PreRet.Line = Line
		return PreRet
	}
}


ObjType := class extend ObjConst
{
	MyType := Type^
	this := !(Type^ st) -> void
	{
		MyType = st
	}
	GetValue := virtual !() -> char^
	{
		return "~type"
	}
	Print := virtual !(int s) -> void
	{
		for s printf("->")
		printf("type %p\n",MyType)
	}
	Clone := virtual !() -> Object^
	{
		PreRet := new ObjType(MyType)
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
ObjTemplateType := class extend Object
{
	MyStr := char^
	this := !(char^ st) -> void
	{
		MyStr = st[1]&.Copy()
	}
	this := !() -> void
	{
	}
	GetValue := virtual !() -> char^
	{
		return "~{}type"
	}
	Print := virtual !(int s) -> void
	{
		for s printf("->")
		printf("template type %s\n",MyStr)
	}
	Clone := virtual !() -> Object^
	{
		PreRet := new ObjTemplateType()
		PreRet.MyStr = MyStr
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

