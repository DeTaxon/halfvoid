
ObjConst := class extend Object
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
		return "~float"
	}
	Print := virtual !(int s) -> void
	{
		for s printf("->")
		printf("double %f\n",MyDouble)
	}	
}

ObjStr := class extend ObjConst
{
	MyStr := char^
	"this" := !(char^ str) -> void
	{
		Clean()
		MyStr = str
	}
	GetValue := virtual !() -> char^
	{
		return "~str"
	}
	Print := virtual !(int s) -> void
	{
		for s printf("->")
		printf("string %s\n",MyStr)
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

