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
	IsConst := virtual true
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
		ResultType = GTypeDouble
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
		f << " " << GetName()
	}
	GetType := virtual !() -> Type^
	{
		return ResultType
	}
	GetName := virtual !() -> string
	{
		if ResultType == GTypeFloat
		{
			preRes := "0x"sbt
			asF := MyDouble
			asI := asF&->{u8^}
			sts := "0123456789ABCDEF"
			for i : 4
			{
				itI := asI[7-i]
				v := (itI >> 4) and_b 0x0F
				preRes << sts[v..1]
				v := itI  and_b 0x0F
				preRes << sts[v..1]
			}
			itI := asI[3] and_b 0xC0
			v := (itI >> 4) and_b 0x0F
			preRes << sts[v..1]
			v := itI  and_b 0x0F
			preRes << sts[v..1]

			preRes << "000000"
			return preRes.Str()
		}
		buf := char[256]
		sprintf(buf,"%f",MyDouble)
		return buf.Copy()
	}
	Clone := virtual !() -> Object^
	{
		PreRet := new ObjDouble(MyDouble)
		PreRet.MyDouble = MyDouble
		PreRet.Line = Line
		return PreRet
	}
}

ObjRangeI := class extend ObjConst
{
	beginR,endR := int
	this := !(int a, int b) -> void
	{
		beginR = a
		endR = b
		ResultType = GTypeRange
	}
	GetValue := virtual !() -> char^
	{
		return "~a..b"
	}
	Print := virtual !(int s) -> void
	{
		for s printf("->")
		printf("range %i..%i\n",beginR,endR)
	}
	PrintUse := virtual !(sfile f) -> void
	{
		f << " @RangeTypeInt {i32 " << beginR << " , i32 " << endR << " }"
	}
	GetName := virtual !() -> string
	{
		buf := char[256]
		sprintf(buf," @RangeTypeInt {i32 %i, i32 %i}",beginR,endR)
		return buf.Copy()
	}
	Clone := virtual !() -> Object^
	{
		PreRet := new ObjRangeI(beginR,endR)
		PreRet.Line = Line
		return PreRet
	}
}

ObjStr := class extend ObjConst
{
	MyStrId := int
	justStr := string
	MyTmpId := int
	this := !(char^ str) -> void
	{
		MyTmpId = GetNewId()
		justStr = str
		ResultType = GTypeString
		MyStrId = StrContainer.GetStringValue(str)
	}
	this := !(int SId,char^ str) -> void
	{
		MyStrId = SId
		MyTmpId = GetNewId()
		ResultType = GTypeString
		justStr = str
	}
	GetString := virtual !() -> string
	{
		return justStr
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
		StrSi := StrSize(justStr) + 1
		f << "%T" << MyTmpId <<" = getelementptr ["<< StrSi << " x i8] , [" << StrSi << " x i8]* @Str" << MyStrId <<", i32 0,i32 0\n" 
	}
	GetConstInline := !() -> char^
	{
		StrSi := StrSize(justStr) + 1
		asStr := "getelementptr inbounds (["sbt + StrSi + " x i8] , [" + StrSi + " x i8]* @Str" + MyStrId + ", i32 0, i32 0)"
		return asStr.Str() ; $temp
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
		PreRet := new ObjStr(MyStrId,justStr)
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
		ResultType = ToUs
	}		
	

	PrintGlobal := virtual !(sfile f) -> void
	{
		asBase := itType->{Type^}
		arrItm := asBase.Base
		for itType.Size Items[it].PrintGlobal(f)
		f << "@Arr" << MyTmpId << " = global " << asBase.GetName() << " ["
		for i : itType.Size 
		{
			if i > 0 f << " , "
			itm := Items[i]
			if arrItm == GTypeDouble and itm is ObjInt
			{
				asInt := itm->{ObjInt^}.MyInt
				f << arrItm.GetName() << " " << asInt << ".0"
			}else if arrItm == GTypeDouble and itm.GetType() == GTypeFloat and itm is ObjDouble{
				asFlt := itm->{ObjDouble^}.MyDouble
				f << arrItm.GetName() << " " << asFlt
			}else if arrItm == GTypeString {
				asStr := itm->{ObjStr^}
				f << arrItm.GetName() << " " << asStr.GetConstInline()
			}else{
				f << arrItm.GetName() << " " << itm.GetName()
			}
		}
		f << "]\n"
	}
	PrintPre := virtual !(sfile f) -> void
	{
		f << "%ArrTmp" << MyTmpId << " = load "
		ResultType.PrintType(f)
		f << " , "
		ResultType.PrintType(f)
		f << "* @Arr" << MyTmpId << "\n"
	}
	PrintUse := virtual !(sfile f) -> void
	{
		ResultType.PrintType(f)
		f << " %ArrTmp"	<< MyTmpId
	}
	PrintPointUse := virtual !(sfile f) -> void
	{
		ResultType.PrintType(f)
		f << "* @Arr" << MyTmpId
	}
	GetPointName := virtual !() -> string
	{
		return "@Arr" + MyTmpId
	}
	GetName := virtual !() -> string
	{
		preRet := "["sbt
		for i : itType.Size
		{
			if i > 0 preRet << " , "
			preRet << Items[i].GetType().GetName() << " " << Items[i].GetName()
		}
		preRet << "]"
		return preRet.Str() ; $temp
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
	DoTheWork := virtual !(int pri) -> void
	{
		if MyStr == "#critical_atomic"
		{
			ReplaceNode(this&,new CriticalAtomic())
		}
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
		MyStr = st
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

