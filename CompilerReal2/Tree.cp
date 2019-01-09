#import "Type.cp"
#import "CommonObjects.cp"
#import "StateParse.cp"
#import "ObjIndent.cp"
#import "WayControl.cp"
#import "StandartObjects.cp"
#import "Syntax.cp"
#import "OMPStyle.cp"


ObjectsPool := StupidMemoryPool.{64000000}

Object := class{
	Left,Right,Down,Up := Object^
	Line :=  ObjLine^

	IsInvalid := bool
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

		iterW := Down
		while iterW != null
		{
			if iterW.Up != this& printf("broken Up %s\n",iterW.GetValue())
			iterW = iterW.Right
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
		if Line != null
		{
			ErrorLog.Push("Error in file <" + Line.inFile.itStr + "> at <" +Line.LinePos + ">:" + err)
		}else{
			ErrorLog.Push(err)
		}
	}
	PrintGlobalSub := !(sfile f) -> void
	{
		iter := Down
		while iter != null
		{
			iter.PrintGlobal(f)
			iter = iter.Right
		}
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
		Up = nUp
		if Right != null Right.SetUp(nUp)
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
		return " at line " + LinePos + " in file " + inFile.itStr
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
	return false
}

