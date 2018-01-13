

ObjResult := class extend Object
{
	ResultType := Type^
}

ObjConst := class extend ObjResult
{
	IsConst := virtual !() -> bool
	{
		return true
	}
}

ObjInt := class extend ObjConst
{
	MyInt := s64
	"this" := !(int Value) -> void
	{
		MyInt = Value
		Clean()
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
}
ObjDouble := class extend ObjConst
{
	MyDouble := double
	"this" := !(double Value) -> void
	{
		MyDouble = Value
		Clean()
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
	DoTheWork := virtual !(int pri) -> void
	{
		//noting is ok
	}
	PrintPre := virtual !(sfile f) -> void
	{
		StrSi := StrContainer.GetString(MyStrId).Size() + 1
		f << "%T" << MyTmpId <<" = getelementptr ["<< StrSi << " x i8] , [" << StrSi << " x i8]* @Str" << MyStrId <<", i32 0,i32 0\n" 
	}
	PrintUse := virtual !(sfile f) -> void
	{
		f << "i8* %T" << MyTmpId	
	}
}
ObjIndent := class extend ObjConst
{
	MyStr := char^
	"this" := !(char^ str) -> void
	{
		Clean()
		MyStr = str
	}
	GetValue := virtual !() -> char^
	{
		return "~ind"
	}
	Print := virtual !(int s) -> void
	{
		for s printf("->")
		printf("indent %s\n",MyStr)
	}
	DoTheWork := virtual !(int pri) -> void
	{
		//noting is ok
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
}

ObjSuffix := class extend ObjConst
{
	MyStr := char^
	"this" := !(char^ str) -> void
	{
		Clean()
		MyStr = str
	}
	GetValue := virtual !() -> char^
	{
		return "~suffix"
	}
	Print := virtual !(int s) -> void
	{
		for s printf("->")
		printf("suffix %s\n",MyStr)
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
}

ObjObj := class extend Object
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
		printf("object %s\n",MyStr)

		iter := Down
		while iter != null {
			iter.Print(s+1)
			iter = iter.Right
		}
	}
}


