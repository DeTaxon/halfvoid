#import "Tree.cp"
#import "BoxClass.cp"

Type := class {
	Id := int
	Base := Type^
	AsPoint := Type^
	AsFatArr := Type^
	AsArray := Queue.{TypeArr^}
	ItName := string
	
	this := !() -> void
	{
		Clean()
	}
	Clean := !() -> void
	{
		Base = null
		AsPoint = null
		AsArray.Start = null
	}
	GetType := virtual !() -> string
	{
		return ""
	}

	GetFatArray := virtual !() -> Type^
	{
		if AsFatArr == null{
			AsFatArr = new TypeFatArr(this&)
		}
		return AsFatArr
	}
	GetPoint := virtual !() -> Type^
	{
		if AsPoint == null{
			AsPoint = new TypePoint(this&)
		}
		return AsPoint
	}
	GetArray :=  !(int size) -> TypeArr^
	{
		iterE := AsArray.Start

		while iterE != null
		{
			if iterE.Data.Size == size return iterE.Data
			iterE = iterE.Next
		}

		NewArr := new TypeArr(this&,size)
		AsArray.Push(NewArr)
		return NewArr
	}
	GetNewName := virtual !() -> string
	{
		return ""
	}
	GetName := virtual !() -> string
	{
		if ItName == null
			ItName = GetNewName()
		return ItName
	}
	PrintType := virtual !(sfile f) -> void
	{
		f << GetName()
	}
	GetAlign := virtual !() -> int
	{
		return 0
	}

}
ParseType := !(Object^ Node) -> Type^ 
{
	if Node == null return null
	if Node.GetValue() == "~ind"
	{
		indName := (Node->{ObjIndent^}).MyStr
		
		NodeName := GetItem(indName,Node)

		if NodeName == null {
			return null
		}
		//NodeName.GetValue()
		if NodeName.GetValue() == ":=type"
		{
			return NodeName->{TypeDef^}.ItType
		}
		if NodeName.Down != null
		{
			if NodeName.Down.GetValue() == "{...}"
			{
				asCl := ((NodeName.Down)->{BoxClass^})
				return asCl.ClassType
			}
		}
		if NodeName.GetValue() == "~type"
		{	
			asNeed := NodeName->{ObjType^}
			return asNeed.MyType
		}
		return null
	}
	if Node.GetValue() == "~type"
	{
		asNeed := Node->{ObjType^}
		return asNeed.MyType
	}
	if Node.GetValue() == "~d"
	{
		lazy := Node.Down != null
		if lazy lazy = Node.Down.GetValue() == "!"
		if lazy lazy = Node.Down.Right.GetValue() == "()"
		if lazy lazy = Node.Down.Right.Right.GetValue() == "^"  or Node.Down.Right.Right.GetValue() == "&"
		if lazy
		{
			types := Queue.{Type^}()
			isVARR := false
			isLambda := false
			ptrSize := 0

			SyntaxCompress(Node.Down.Right,PriorityData)
			UnboxParams(Node.Down.Right)

			iter := Node.Down.Right.Down

			while iter != null
			{
				if iter.GetValue() != ","
				{
					someType := ParseType(iter)
					if someType == null
						return null
					types.Push(someType)
				}
				iter = iter.Right 
			}
			iter = Node.Down.Right.Right
			while iter.GetValue() == "^"
			{
				ptrSize += 1
				iter = iter.Right
			}
			if iter.GetValue() == "&"
			{
				isLambda = true
				iter = iter.Right
			}

			iter = iter.Right
			someType := ParseType(iter)
			if someType == null return null

			if isLambda 
			{
				types.PushFront(TypeTable[11].GetPoint())
				someType = GetFuncType(types,null->{bool^},someType,false,isVARR)
				asN := someType->{TypeFunc^}
				return asN.GetLambda()
			}

			someType = GetFuncType(types,null->{bool^},someType,false,isVARR)
			for ptrSize someType = someType.GetPoint()
			return someType
		}
		
		lazy = Node.Down.GetValue() == "~ind"
		if lazy lazy = Node.Down.Right != null
		if lazy lazy = Node.Down.Right.GetValue() == "."
		if lazy lazy = Node.Down.Right.Right != null
		if lazy lazy = Node.Down.Right.Right.GetValue() == "{}"
		if lazy 
		{
			SyntaxCompress(Node.Down.Right.Right,PriorityData)
			itName := ((Node.Down)->{ObjIndent^}).MyStr
			NodeName := GetItem(itName,Node)

			if NodeName == null return null
			if NodeName.Down.GetValue() == "!{}{...}"
			{
				asT := ((NodeName.Down)->{BoxClassTemplate^})
				Objs := Queue.{Object^}()
				
				iterR := Node.Down.Right.Right.Down
				
				while iterR != null
				{	
					if iterR.GetValue() != ","
					{
						isType := ParseType(iterR)
						if isType == null
						{
							val := TryCompute(iterR)
							if val != null Objs.Push(val)
							else 
							{
								Node.EmitError("can not create class\n")
							}
						}else{
							Objs.Push(new ObjType(isType))
						}
					}
					iterR = iterR.Right
				}
				if Objs.Size() == 0 return null
				return asT.GetClass(Objs)
			}
			return null
		}



		if true
		{
			if not InDataR(Node.Down) return null
			Ri := Node.Down.Right
			if Ri == null return ParseType(Node.Down)
			
			under := ParseType(Node.Down)
			if under == null return null

			if Ri.GetValue() == "^"
			{
				return under.GetPoint()
			}
			if Ri.GetValue() == "[]"
			{
				if Ri.Down == null
				{
					return under.GetFatArray()
				}else
				{
					val := Object^
					val = null
					//if Ri.Down.GetValue() == "~ind"
					//{
					//	asNeed := ((Ri.Down)->{ObjIndent^})
					//	for i : nams.Size() 
					//		if nams[i] == asNeed.MyStr[1]& 
					//			val = vals[i]
					//}
					if val == null val = TryCompute(Ri.Down)
					if val == null return null
					if val.GetValue() == "~int"
					{
						DynCast := val->{ObjInt^}
						ArrSize := DynCast.MyInt
						return under.GetArray(ArrSize)->{Type^}
					}
				}

				return null
			}
		}


	}
	return null
}

FuncTypeTable := Queue.{TypeFunc^}


ExchangeFuncType := !(TypeFunc^ FType,Type^ retType) -> TypeFunc^
{
	iterT := FuncTypeTable.Start

	while iterT != null
	{
		if iterT.Data.IsVArgs == FType.IsVArgs
			and iterT.Data.ParsCount == FType.ParsCount
			and iterT.Data.RetType == retType
		{
			IsFound := true

			for FType.ParsCount
			{
				if FType.Pars[it] != iterT.Data.Pars[it] IsFound = false 
				if FType.ParsIsRef[it] != iterT.Data.ParsIsRef[it] IsFound = false 
			}
			if IsFound
			{
				return iterT.Data
			}
		}
		iterT = iterT.Next
	}
	newTypeFunc := new TypeFunc(FType)
	newTypeFunc.RetType = retType
	FuncTypeTable.Push(newTypeFunc)
	return newTypeFunc

}

GetFuncType := !(Queue.{Type^} lin,bool^ IsRefArr,Type^ retType, bool retRef, bool IsVArgs) -> TypeFunc^
{
	iterT := FuncTypeTable.Start

	ExtraArr := new bool[lin.Size()]
	for i : lin.Size()
	{
		ExtraArr[i] = false
		if IsRefArr != null ExtraArr[i] = IsRefArr[i]
		if lin[i] != null
		{
			if lin[i].GetType() == "class" ExtraArr[i] = true
			if lin[i].GetType() == "arr" ExtraArr[i] = true
		}
	}

	while iterT != null
	{
		SomeBug := IsVArgs
		SomeBug2 := retRef
		if iterT.Data.IsVArgs == SomeBug and SomeBug2 == iterT.Data.RetRef
		{
			IsFound := true
			if iterT.Data.ParsCount == lin.Size() //VArg?
			{
				iterR := lin.Start
				i := 0
				while iterR != null
				{
					if iterT.Data.Pars[i] != iterR.Data IsFound = false

					if i < lin.Size()
						if iterT.Data.ParsIsRef[i] != ExtraArr[i] IsFound = false
					i += 1
					iterR = iterR.Next 
				}
			}else{
				IsFound = false
			}
			if iterT.Data.RetType != retType
			{
				IsFound = false
			}
			if IsFound return iterT.Data
		}
		iterT = iterT.Next
	}

	newTypeFunc := new TypeFunc(lin,ExtraArr,IsVArgs)
	newTypeFunc.RetType = retType
	newTypeFunc.RetRef = retRef
	FuncTypeTable.Push(newTypeFunc)
	return newTypeFunc
}

TypeDef := class extend Object
{
	ItName := string
	ItType := Type^

	this := !(string name, Type^ T) -> void
	{
		ItName = name
		ItType = T
	}
	GetValue := virtual !() -> string
	{
		return ":=type"
	}
}

TypeStandart := class extend Type{
	IRName := string
	ItSize  := int
	ItAlign := int
	
	this := !(string Name, int siz, int Aln) -> void
	{
		IRName = Name 
		ItSize = siz
		ItAlign = Aln
		Clean()
	}
	GetType := virtual !() -> string
	{
		return "standart"
	}
	GetName := virtual !() -> string
	{
		return IRName
	}
	GetAlign := virtual !() -> int
	{
		return ItAlign
	}
}

TypePoint := class extend Type
{
	this := !(Type^ nBase) -> void
	{
		Clean()
		Base = nBase
	}
	GetType := virtual !() -> string
	{
		return "point"
	}
	GetNewName := virtual !() -> string
	{
		return Base.GetName() + "*"
	}
	GetAlign := virtual !() -> int
	{
		return 8
	}
}
TypePointVoidP := class extend TypePoint
{
	this := !(Type^ nBase) -> void
	{
		Base = nBase
	}
	GetName := virtual !() -> string
	{
		return "i8*"
	}
}
TypePointVoidFatP := class extend TypeFatArr
{
	this := !(Type^ nBase) -> void
	{
		Base = nBase
	}
	GetName := virtual !() -> string
	{
		return "i8*"
	}
}

TypeFunc := class extend Type
{
	ParsCount := int
	Pars := Type^^
	ParsIsRef := bool^
	IsVArgs := bool
	RetType := Type^
	RetRef := bool

	asLambda := Type^

	this := !(TypeFunc^ FType) -> void
	{
		ParsCount = FType.ParsCount

		ParsIsRef = new bool[ParsCount]
		for i : ParsCount ParsIsRef[i] = FType.ParsIsRef[i]

		Pars = null
		if ParsCount != 0 {
			Pars = new Type^[ParsCount]
			for ParsCount Pars[it] = FType.Pars[it]
		}
		RetRef = false
		IsVArgs = FType.IsVArgs
	}
	this := !(Queue.{Type^} P,bool^ retsRef, bool IsV) -> void
	{
		ParsCount = P.Size()

		if retsRef != null
		{
			ParsIsRef = new bool[ParsCount]
			for i : ParsCount ParsIsRef[i] = retsRef[i]
		}else{
			if ParsCount != 0
			{
				ParsIsRef = new bool[ParsCount]
				for i : ParsCount ParsIsRef[i] = 0
			}
		}
		Pars = null
		if ParsCount != 0 Pars = P.ToArray()
		RetRef = false
		IsVArgs = IsV
	}
	GetType := virtual !() -> string
	{
		return "function"
	}
	GetNewName := virtual !() -> string
	{
		if RetType == null
		{
			return ""
		}
		if (RetType.GetType() == "arr" or RetType.GetType() == "class") and not RetRef
		{
			return "void" + GetSkobs() 
		}
		if RetRef return RetType.GetName() + "*" + GetSkobs()
		return RetType.GetName() + GetSkobs() 
	}
	GetLambda := !() -> Type^
	{
		if asLambda == null
		{
			asLambda = new TypeFuncLambda(this&->{Type^})
		}
		return asLambda
	}
	GetSkobs := !() -> string
	{
		ToRet := string
		ToRet = ""
		ToRet = ToRet + "("
		
		if RetType != null
		{
			if RetType.GetType() == "arr" or RetType.GetType() == "class"
			{
				if not RetRef
				{
					ToRet = ToRet + RetType.GetName() + "* "
					if ParsCount > 0 ToRet = ToRet + " , "
				}
			}
		}


		for i : ParsCount
		{
			if i != 0 ToRet = ToRet +  " , "
			if ParsIsRef[i]	ToRet = ToRet + Pars[i].GetPoint().GetName()
			else ToRet = ToRet + Pars[i].GetName()
		}
		if IsVArgs
		{
			if ParsCount != 0 ToRet = ToRet +  " , "
			ToRet = ToRet + "..."
		}
		ToRet = ToRet + ")"
		return ToRet
	}
	PrintSkobs := virtual !(sfile f) -> void
	{
		f << GetSkobs()
	}
	PrintType := virtual !(sfile f) -> void
	{
		f << GetName()
	}
	GetAlign := virtual !() -> int
	{
		return 8
	}
}

TypeFuncLambda := class extend Type
{
	almostName := string
	this := !(Type^ asBase) -> void
	{
		Base = asBase
	}
	PrintType := virtual !(sfile f) -> void
	{
		f << GetName()
	}
	GetPointName := !() -> string
	{
		if almostName == null
			almostName = Base.GetName() + "*"
		return almostName
	}
	GetNewName := virtual !() -> string
	{
		return Base.GetName() + "**"
		//return GetNewNamePre() + "*"
	}
	GetNewNamePre := virtual !() -> string
	{
		asB := Base->{TypeFunc^}

		ToRet := ""

		IsCompl := false

		if not asB.RetRef
		{
			IsCompl = IsComplexType(asB.RetType)
		}

		if IsCompl {
			ToRet = ToRet + "void"
		}else{
			ToRet = ToRet + asB.RetType.GetName()
		}
		ToRet = ToRet + "(i8*"

		if IsCompl or asB.ParsCount != 0 ToRet = ToRet + ","

		if IsCompl{
			ToRet = ToRet + asB.RetType.GetName() + "*"			
			if asB.ParsCount != 0 ToRet = ToRet + ","
		}
		
		for i : asB.ParsCount
		{
			if i > 0 ToRet = ToRet + ","
			ToRet = ToRet + asB.Pars[i].GetName()
		}
		ToRet = ToRet + ")*"
		return ToRet	
	}
	GetAlign := virtual !() -> int
	{
		return 8
	}
	GetType :=  virtual !() -> string
	{
		return "lambda"
	}
}

TypeArr := class extend Type
{
	Size := int

	this := !(Type^ B, int S) -> void
	{
		Base = B
		Size = S
	}
	GetType := virtual !() -> string
	{
		return "arr"
	}
	GetNewName := virtual !() -> string
	{
		return "[" + Size + " x " + Base.GetName() + "]"
	}
	GetAlign := virtual !() -> int
	{
		return Base.GetAlign()
	}
}
TypeFatArr := class extend Type
{
	this := !(Type^ B) -> void
	{
		Base = B
	}
	GetType := virtual !() -> string
	{
		return "fatarr"
	}
	GetNewName := virtual !() -> string
	{
		return Base.GetName() + "*"
	}
	GetAlign := virtual !() -> int
	{
		return 8
	}
}
TypeClass := class extend Type
{
	ToClass := BoxClass^
	this := !(BoxClass^ ToSet) -> void
	{
		ToClass = ToSet
	}
	GetType := virtual !() -> string
	{
		return "class"
	}
	GetNewName := virtual !() -> string
	{
		return "%Class" + ToClass.ClassId
	}
	GetAlign := virtual !() -> int
	{
		maxVal := 1

		iter := ToClass.Params.Start

		while iter != null
		{
			al := iter.Data.ResultType.GetAlign()

			if maxVal < al maxVal = al
			iter = iter.Next
		}
		return maxVal
	}
}

GetType := !(string Name) -> Type^
{
	for Def : DefsTable
	{
		if Def.ItName == Name return Def.ItType
	}
	return null
}

CreateStandartTypes := !() -> void
{	
	//u
	TypeTable[0] = new TypeStandart("i8",1,1)	
	TypeTable[1] = new TypeStandart("i16",2,2)	
	TypeTable[2] = new TypeStandart("i32",4,4)	
	TypeTable[3] = new TypeStandart("i64",8,8)

	//s
	TypeTable[4] = new TypeStandart("i8",1,1)	
	TypeTable[5] = new TypeStandart("i16",2,2)	
	TypeTable[6] = new TypeStandart("i32",4,4)	
	TypeTable[7] = new TypeStandart("i64",8,8)

	TypeTable[8] = new TypeStandart("i1",1,1)	// bool

	TypeTable[9] = new TypeStandart("float",4,4)
	TypeTable[10] = new TypeStandart("double",8,8)
	TypeTable[11] = new TypeStandart("void",0,0)

	TypeTable[12] = TypeTable[0].GetPoint() // char*

	VoidPType = new TypePointVoidP(TypeTable[11]) // void*
	TypeTable[11].AsPoint = VoidPType

	TypeTable[16] = new TypePointVoidFatP(TypeTable[11])
	TypeTable[11].AsFatArr = TypeTable[16]

	GlobalStrs = GlobalStrs + "%RangeTypeInt = type {i32,i32}\n"
	 			+ "%RangeTypeFloat = type {float,float}\n"

	TypeTable[13] = new TypeStandart("%RangeTypeInt",8,8)
	TypeTable[13].Base = TypeTable[6]
	TypeTable[14] = new TypeStandart("%RangeTypeFloat",8,8)
	TypeTable[14].Base = TypeTable[9]

	GlobalStrs = GlobalStrs + "%OpaqType = type {i1}\n"
	TypeTable[15] = new TypeStandart("%OpaqType",0,0)

	GlobalStrs = GlobalStrs + "%Vec4f = type <4 x float>\n"
	TypeTable[17] = new TypeStandart("%Vec4f",16,16)
	TypeTable[18] = new TypeStandart("%Vec4f",16,16)



	DefsTable[0] = new TypeDef("u8",TypeTable[0])
	DefsTable[1] = new TypeDef("u16",TypeTable[1])
	DefsTable[2] = new TypeDef("u32",TypeTable[2])
	DefsTable[3] = new TypeDef("u64",TypeTable[3])

	DefsTable[4] = new TypeDef("s8",TypeTable[4])
	DefsTable[5] = new TypeDef("s16",TypeTable[5])
	DefsTable[6] = new TypeDef("s32",TypeTable[6])
	DefsTable[7] = new TypeDef("s64",TypeTable[7])

	DefsTable[8] = new TypeDef("bool",TypeTable[8])
	DefsTable[9] = new TypeDef("char",TypeTable[0])
	DefsTable[10] = new TypeDef("short",TypeTable[5])
	DefsTable[11] = new TypeDef("int",TypeTable[6])
	DefsTable[12] = new TypeDef("long",TypeTable[7])
	DefsTable[13] = new TypeDef("float",TypeTable[9])
	DefsTable[14] = new TypeDef("double",TypeTable[10])

	DefsTable[15] = new TypeDef("void",TypeTable[11])

	DefsTable[16] = new TypeDef("string",TypeTable[12])

	DefsTable[17] = new TypeDef("opaque",TypeTable[15])

	DefsTable[18] = new TypeDef("range",TypeTable[13])
	DefsTable[19] = new TypeDef("rangef",TypeTable[14])

	DefsTable[20] = new TypeDef("vec4f",TypeTable[17])
	DefsTable[21] = new TypeDef("quantf",TypeTable[18])
}
IsInt := !(Type^ tp) -> bool
{
	for 8 if TypeTable[it] == tp return true
	return false
}
IsFloat := !(Type^ tp) -> bool
{
	if TypeTable[9] == tp return true
	if TypeTable[10] == tp return true
	return false
}
IsBool := !(Type^ tp) -> bool
{
	if TypeTable[8] == tp return true
	return false
}

GetIntSize := !(Type^ tp) -> int
{
	i := 8
	for 4 
	{
		if TypeTable[it] == tp return i
		i = i*2
	}
	i = 8
	for 4 
	{
		if TypeTable[it + 4] == tp return i
		i = i*2
	}
	return 0
}

IsComplexType := !(Type^ typ) -> bool
{
	t := typ.GetType()
	if t == "arr" return true
	if t == "class" return true
	return false
}

