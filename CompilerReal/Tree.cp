
Object := class{
	Id := int
	Left,Right,Down,Up := Object^
	Line :=  Object^

	IsInvalid := bool

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

	Clean := !() -> void
	{
		Left = null
		Right = null
		Down = null
		Up = null
		Line = null
	}	
	"this" := !() -> void
	{
		Id = 0
		Clean()
	}
	"this" := !(int id) -> void
	{
		Id = id
		Clean()
	}
	IsConst := virtual !() -> bool
	{
		return false
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

	DoTheWork := virtual !(int st) -> void
	{
		printf("work not specefied for %s\n",GetValue())
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
		free(where)
	}else if where.Left == null
	{
		where.Up.Down = where.Right
		where.Right.Left = null
		free(where)
	}else //if where.Right == null
	{
		where.Left.Right = where.Right
		if where.Right != null where.Right.Left = where.Left
		free(where)
	}
}

UNext := !(Object^ where,Object^ nObj, int count) -> Object^
{
	Last := where
	for count-1 Last = Last.Right
	UNext(where,nObj,Last)
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

IsKeyword := !(char^ W) -> bool
{
	if W == "return" return true
	if W == "for" return true
	if W == "if" return true
	if W == "else" return true
	if W == "while" return true
	if W == "do" return true
	if W == "new" return true
	if W == "delete" return true
	if W == "switch" return true
	if W == "case" return true
	return false
}

TokensToObjects := !(char^ filename, Queue.{Token^} Toks) -> Object^
{
	DaFile := new BoxBlock()
	iter := DaFile->{Object^}
	Adder := DaFile->{Object^}
	
	while Toks.NotEmpty()
	{
	// word-1 Hex-2 int-3 int_s-4 float-5 float_s-6 floate-7 floate_s-8 str 9 #-10 \n-11
		Tok := Toks.Pop()
		if Tok.Id == 11
		{
			NL := new Object(0)
			while iter.Right != null
			{
				iter = iter.Right
				iter.Line = NL
			}
		}
		if Tok.Id == 10
		{
			NS := new ObjSymbol(Tok.Buff.Copy())
			Adder = PushObject(Adder,NS)
		}
		if Tok.Id == 9
		{
			DaBuff := char[2048]
			j := 0
			k := 1
			while Tok.Buff[k] != '"'
			{
				if Tok.Buff[k] == '\\' {
					k += 1
					if Tok.Buff[k] == 'n' DaBuff[j] = 10
					else 
					{
						DaBuff[j]  = Tok.Buff[k]
					}
					j += 1
					k += 1
				}else{
					DaBuff[j] = Tok.Buff[k]
					k += 1
					j += 1
				}	
			}
			DaBuff[j] = 0
			NS := new ObjStr(DaBuff.Copy())
			Adder = PushObject(Adder,NS)
		}
		if Tok.Id == 1
		{ 
			if Tok.Buff[0] in "$@#"
			{
				if Tok.Buff[0] == '#'
				{
					NS := new ObjCmd(Tok.Buff.Copy())
					Adder = PushObject(Adder,NS)
				}else{
					NS := new ObjCmd(Tok.Buff.Copy())
					Adder = PushObject(Adder,NS)
				}
			}else{
				if IsKeyword(Tok.Buff)
				{
					NS := new ObjKeyword(Tok.Buff.Copy())
					Adder = PushObject(Adder,NS)
				}else{
					NS := new ObjIndent(Tok.Buff.Copy())
					Adder = PushObject(Adder,NS)
				}
			}
		}
		if Tok.Id == 2
		{
			Value := int
			
			Value = 0
			
			k := 2
		
			while Tok.Buff[k]
			{
				Value *= 16
				if Tok.Buff[k] in '0'..'9' Value += Tok.Buff[k] - '0'
				if Tok.Buff[k] in 'A'..'F' Value += Tok.Buff[k] - 'A' + 10
				if Tok.Buff[k] in 'a'..'f' Value += Tok.Buff[k] - 'a' + 10
				k += 1
			}
			NV := new ObjInt(Value)
			Adder = PushObject(Adder,NV->{Object^})
		}
		if Tok.Id == 3 or Tok.Id == 4
		{
			Value := int
			
			Value = 0
			
			k := 0
		
			while Tok.Buff[k] in '0'..'9'
			{
				Value *= 10
				Value += Tok.Buff[k] - '0'
				k += 1
			}
			NV := new ObjInt(Value)
			Adder = PushObject(Adder,NV->{Object^})

			if Tok.Id == 4
			{
				NS := new ObjSuffix(Tok.Buff[k]&.Copy())
				Adder = PushObject(Adder,NS)
			}
		}
		if Tok.Id in 5..8
		{
			Value := 0.0
			k := 0
			Quant := 0.1
			SSing := 1.0

			while Tok.Buff[k] != '.'
			{
				Value *= 10
				Value += Tok.Buff[k] - '0' 
				k += 1 
			}
			k += 1 //.
			while Tok.Buff[k] in '0'..'9'
			{
				Value += (Tok.Buff[k] - '0')*Quant
				k += 1
				Quant *= 0.1
			}
			if Tok.Buff[k] == 'e' 
			{
				// not now
			}
			NV := new ObjDouble(Value->{double})
			Adder = PushObject(Adder,NV->{Object^})

			if Tok.Buff[k] != 0
			{
				NS := new ObjSuffix(Tok.Buff[k]&.Copy())
				Adder = PushObject(Adder,NS)
			}
			
		}
		free(Tok)
	}
	DaFile.Down = DaFile.Right
	iter = DaFile.Down
	iter.Left = null

	while iter != null
	{
		iter.Up = DaFile
		iter = iter.Right
	}
	return DaFile
}

