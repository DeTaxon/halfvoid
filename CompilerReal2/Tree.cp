ObjectsPool := AllocOnlyMP.{8388608,true}

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
			preRes = gTemporaryPool.GetMem(R->TypeSize,R->Align,0)
		}else{
			preRes = ObjectsPool.GetMem(R->TypeSize,R->Align,0)
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
	GetDebugValue := virtual !() -> char^
	{
		return GetValue()
	}
	Print := virtual !(int s) -> void {
		for s printf("->")
		printf("item: %s",GetDebugValue())
		if Line != null
		{
			printf(" at %s:%i",Line.inFile.Get(),Line.LinePos)
		}
		printf("\n")
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
			sbtt << "Error in " << Line.inFile.itStr << ":" << Line.LinePos << " "
		}
		sbtt << err
		sbtt << "\n"
		if workingOnObject?.Line != null
		{
			sbtt << "Called from " << workingOnObject.Line.inFile.itStr << ":" << workingOnObject.Line.LinePos << "\n"
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
		outRes := sbtt.Str()
		ErrorLog.Push(outRes)
		throw new Exception(outRes)
	}
	PrintGlobalSub := !(TIOStream f) -> void
	{
		Down[^].PrintGlobal(f)
	}
	PrintGlobal := virtual !(TIOStream f) -> void
	{
		PrintGlobalSub(f)
	}
	PrintDestructor := virtual !(TIOStream f) -> void
	{
	}
	GetOutPath := virtual !(Object^ from,int typ,int size) -> BoxLabel^
	{
		if Up != null
			return Up.GetOutPath(from,typ,size)
		return null
	}
	PrintInBlock := virtual !(TIOStream f) -> void
	{
		ErrorLog.Push("Compiler error: PrintInBlock is not defined")	
	}
	PrintPointPre := virtual !(TIOStream f) -> void
	{
		ErrorLog.Push("Compiler error: PrintPointPre is not defined")	
	}
	PrintPointUse := virtual !(TIOStream f) -> void
	{
		ErrorLog.Push("Compiler error: PrintPointUse is not defined")	
	}
	PrintPre := virtual !(TIOStream f) -> void
	{
		ErrorLog.Push("Compiler error: PrintPre is not defined")	
	}
	PrintUse := virtual !(TIOStream f) -> void
	{
		ErrorLog.Push("Compiler error: PrintUse is not defined")	
	}
	GetName := virtual !() -> char^
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
		ErrorLog.Push("Clone not defined for "sbt + GetValue() + "\n")
		return null
	}
	ApplyDeferUse := virtual !(int depth) -> void
	{
		if Up != null
			Up.ApplyDeferUse(depth)
	}
	PrintDebugDeclare := virtual !(TIOStream f ,Object^ frc,char^ forcedName) -> void
	{
		Down[^].PrintDebugDeclare(f,frc,null->{char^})
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
	IsDataR := virtual false
	IsBlockData := virtual false

	GetTokenId := virtual !() -> int
	{
		return 0
	}
	GetEndLabel := virtual !() -> BoxLabel^
	{
		return null
	}
	GetBadLabel := virtual !(Object^ prevNode) -> BoxLabel^
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
		return " at  "sbt + inFile.itStr + ":" + LinePos
	}
	LoadAttrs := !(char^ str) -> void
	{
		newItm := DivideStr(str,"\t\n ")
		for newItm
		{
			if it != ";"
			{
				itAttrs[it.Str()] = new ObjBool(true)
			}
		}
	}
	"new" := !() .{@R} -> void^
	{
		return ObjectsPool.GetMem(R->TypeSize,R->Align,0)
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
	switch W
	{
		case "return" return true
		case "for" return true
		case "if" return true
		case "else" return true
		case "while" return true
		case "do" return true
		case "delete" return true
		case "switch" return true
		case "case" return true
		case "class" return true
		case "extern" return true
		case "extend" return true
		case "at" return true
		case "defer" return true
		case "type" return true
		case "virtual" return true
		case "new" return true
		case "delete" return true
		case "thread_local" return true
		case "task_local" return true
		case "keep_name" return true
		case "packed_class" return true
		case "self_return" return true
		case "try" return true
		case "catch" return true
		case "throw" return true
		case "on_exception" return true
		case "yield" return true
		case "AppendClass" return true
		case "default" return true
		case "weak" return true
		case "poison" return true
	}

	return false
}

