ObjectsPool := StupidMemoryPool.{8388608}

Object := class{
	Left,Right,Down,Up := Object^
	Line :=  ObjLine^

	inhAttrs := AttrArrayType^

	visitedWork := Bitset.{4}


	IsConst := virtual false

	"new" := !() .{@R} -> void^
	{
		preRes := void^
		if $temp {
			preRes = gTemporaryPool.GetMem(R->TypeSize,R->Align)
		}else{
			preRes = ObjectsPool.GetMem(R->TypeSize,R->Align)
		}
		return preRes
	}
	
	TestNodes := virtual !() ->void
	{
		if Up == null printf("broken %s\n",GetValue())
		if Right != null Right.TestNodes()
		if Down != null Down.TestNodes()

		if Down[^].Up != this&
		{
			if it is SLambda and this& is GlobalParam
				continue
			this.Print(0)
			printf("broken Up %s\n",it.GetValue())
		}
	}

	this := !() -> void
	{
	}
	IsRef := virtual !() -> bool
	{
		return false
	}
	GetValue := virtual !() -> char^
	{
		return "~unknown"
	}
	Print := virtual !(int s) -> void {
		for s printf("->")
		printf("item: %s\n",GetValue())
		End := this.Down
		while End != null
		{
			End.Print(s+1)
			End = End.Right
		}
	}
	EmitError := !(string err) -> void
	{
		sbtt := ""sbt
		if Line != null
		{
			sbtt << "Error in file <" << Line.inFile.itStr << "> at <" << Line.LinePos << ">:"
		}
		sbtt << err
		sbtt << "\n"
		if workingOnObject?.Line != null
		{
			sbtt << "Called from <" << workingOnObject.Line.inFile.itStr << "> at <" << workingOnObject.Line.LinePos << ">\n"
		}

		itr := Up
		while itr != null
		{
			if itr is BoxFuncBody
				break
			itr = itr.Up
		}
		if itr?.Up? is BoxTemplate
		{
			asB := itr->{BoxFuncBody^}
			sbtt << "template type " << asB.MyFuncType.GetGoodName() << "\n"
		}
		ErrorLog.Push(sbtt.Str())
	}
	PrintGlobalSub := !(sfile f) -> void
	{
		Down[^].PrintGlobal(f)
	}
	PrintGlobal := virtual !(sfile f) -> void
	{
		PrintGlobalSub(f)
	}
	PrintDestructor := virtual !(sfile f) -> void
	{
	}
	GetOutPath := virtual !(Object^ from,int typ,int size) -> string
	{
		return ""
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		ErrorLog.Push("Compiler error: PrintInBlock is not defined")	
	}
	PrintPointPre := virtual !(sfile f) -> void
	{
		ErrorLog.Push("Compiler error: PrintPointPre is not defined")	
	}
	PrintPointUse := virtual !(sfile f) -> void
	{
		ErrorLog.Push("Compiler error: PrintPointUse is not defined")	
	}
	PrintPre := virtual !(sfile f) -> void
	{
		ErrorLog.Push("Compiler error: PrintPre is not defined")	
	}
	PrintUse := virtual !(sfile f) -> void
	{
		ErrorLog.Push("Compiler error: PrintUse is not defined")	
	}
	GetName := virtual !() -> string
	{
		ErrorLog.Push("Compiler error: GetName is not defined")	
		return ""
	}
	GetPointName := virtual !() -> string
	{
		ErrorLog.Push("Compiler error: GetPointName is not defined")
		return ""
	}
	GetItem := virtual !(string name) -> Object^
	{
		return null
	}

	DoTheWork := virtual !(int st) -> void
	{
		//printf("work not specefied for %s\n",GetValue())
	}
	SetUp := !(Object^ nUp) -> void
	{
		this&[^].Up = nUp
	}
	GetType := virtual !() -> Type^
	{
		return null
	}
	Clone := virtual !() -> Object^
	{
		ErrorLog.Push("Clone not defined for " + GetValue() + "\n")
		return null
	}
	ApplyDeferUse := virtual !(int depth) -> void
	{
		if Up != null
			Up.ApplyDeferUse(depth)
	}
	PrintDebugDeclare := virtual !(sfile f ,Object^ frc) -> void
	{
		Down[^].PrintDebugDeclare(f,frc)
	}
	GetScope := virtual !() -> int
	{
		if Up == null return 0
		return Up.GetScope()
	}
	GetABox := virtual !() -> AllocBox^
	{
		return null
	}
}

ObjLine := class 
{
	LinePos := int
	inFile := Path
	itAttrs := AVLMap.{string,Object^}
	this := !() -> void
	{
		inFile."this"()
		itAttrs."this"()
	}
	this := !(int LP, Path IF) -> void
	{
		LinePos = LP
		inFile = IF
		itAttrs."this"()
	}
	GetLog := !() -> string
	{
		return " at line "sbt + LinePos + " in file " + inFile.itStr
	}
	LoadAttrs := !(char^ str) -> void
	{
		newItm := Queue.{char^}()
		DivideStr(str,' ',newItm)
		for newItm
		{
			if it != ";"
			{
				itAttrs[it] = new ObjBool(true)
			}
		}
	}
	"new" := !() .{@R} -> void^
	{
		return ObjectsPool.GetMem(R->TypeSize,R->Align)
	}
}

ForIteratorOverLineInTree := class
{
	itr := Object^
	this := !(Object^ item) -> void	{ itr = item }
	"^" := !() -> Object^ { return itr }
	Inc := !() -> void { itr = itr.Right }
	IsEnd := !() -> bool { return itr == null }
}
"~For" := !(Object^ itrOver) -> ForIteratorOverLineInTree
{
	return ForIteratorOverLineInTree(itrOver)
}



PushObject := !(Object^ Ad,Object^ ToAdd) -> Object^
{
	Ad.Right = ToAdd
	ToAdd.Left = Ad
	return ToAdd
}


Remove := !(Object^ where ) -> void
{
	if where.Left == null and where.Right == null
	{
		where.Up.Down = null
		//free(where)
	}else if where.Left == null
	{
		where.Up.Down = where.Right
		where.Right.Left = null
		//free(where)
	}else //if where.Right == null
	{
		where.Left.Right = where.Right
		if where.Right != null where.Right.Left = where.Left
		//free(where)
	}
}

UNext := !(Object^ where,Object^ nObj, int count) -> Object^
{
	Last := where
	for count-1 Last = Last.Right
	UNext(where,nObj,Last)
	nObj.Line = where.Line
	return nObj
}
UNext := !(Object^ where,Object^ nObj, Object^ Last) -> void
{
	End := Last.Right

	if End != null {
		End.Left.Right = null
		End.Left = nObj
		nObj.Right = End
	}

	if where.Left == null{
		Upp := where.Up

		Upp.Down = nObj
		nObj.Down = where
		nObj.Up = Upp

	}else{
		Ll := where.Left

		Ll.Right = nObj
		nObj.Left = Ll
		nObj.Down = where
		nObj.Up = Ll.Up
		where.Left = null
	}

	End = where
	while End != null {
		End.Up = nObj
		End = End.Right
	}
}


IsKeyword := !(W) -> bool
{
	//switch W
	//{
		if W == "return" return true
		if W == "for" return true
		if W == "if" return true
		if W == "else" return true
		if W == "while" return true
		if W == "do" return true
		if W == "delete" return true
		if W == "switch" return true
		if W == "case" return true
		if W == "class" return true
		if W == "extern" return true
		if W == "extend" return true
		if W == "at" return true
		if W == "defer" return true
		if W == "type" return true
		if W == "virtual" return true
		if W == "new" return true
		if W == "delete" return true
		if W == "thread_local" return true
		if W == "keep_name" return true
		if W == "packed_class" return true
		if W == "self_return" return true
		if W == "try" return true
		if W == "catch" return true
		if W == "throw" return true
		if W == "on_exception" return true
		if W == "yield" return true
	//}

	return false
}

