TypesPool := StupidMemoryPool.{16000000}

Type := class {
	Id := int
	ItHash := int
	Base := Type^
	AsPoint := Type^
	AsFatArr := Type^
	AsArray := Queue.{TypeArr^}
	ItName := string
	
	"new" := !() .{@R} -> void^
	{
		preRes :=  TypesPool.GetMem(R->TypeSize,R->Align)
		return preRes
	}
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
		if ItName == null->{int^}
		{
			ItName = GetNewName()
		}
		return ItName
	}
	GetGoodName := virtual !() -> string
	{
		return ""
	}

	metaId := int

	PrintType := virtual !(sfile f) -> void
	{
		f << GetName()
	}
	GetAlign := virtual !() -> int
	{
		assert(false)
		return 0
	}
	GetSize := virtual !() -> int
	{
		assert(false)
		return 0
	}
}
ParseType := !(Object^ Node) -> Type^
{
	return ParseType(Node,null)
}
ParseType := !(Object^ Node,AttrArrayType^ toAdd) -> Type^
{
	return ParseType(Node,toAdd,null)
}
ParseType := !(Object^ Node,AttrArrayType^ toAdd,Queue.{ObjConstHolder^}^ tempConsts) -> Type^ 
{
	if Node == null return null
	if Node is ObjIndent
	{
		indName := (Node->{ObjIndent^}).MyStr
		
		NodeName := GetItem(indName,Node)
		
		if tempConsts != null
		{
			if tempConsts^[^].ItName == NodeName and it.Down? is ObjType
			{
				return it.Down->{ObjType^}.MyType
			}
		}

		if NodeName == null {
			for Modules
			{
				tryType := it.GetModuleType(indName)
				if tryType != null return tryType
			}
			for itCodeS : CodeSpaces ; $reverse
			{
				inMMap := itCodeS.codeTypeDefs.TryFind(indName)
				if inMMap != null
				{
					return inMMap^.GetType()
				}
			}
			return null
		}
		//NodeName.GetValue()
		if NodeName.GetValue() == ":=type"
		{
			return NodeName.GetType()
		}
		if NodeName.Down != null
		{
			if NodeName.Down.GetValue() == "{...}"
			{
				asCl := ((NodeName.Down)->{BoxClass^})
				return asCl.ClassType
			}
		}
		if NodeName is ObjType
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
		if Node.Down?.GetValue() == "!" and (Node.Down.Right?.GetValue() == "{}" or Node.Down.Right?.GetValue() == "{d}")
		{
			itBox := new FuncInputBox ; $temp
			for nd : Node.Down.Right.Down
			{
				if nd.GetValue() == ","
					continue
				newTyp := ParseType(nd)
				if newTyp == null
					return null
				itBox.itPars.Emplace(newTyp,false)
			}
			if itBox.itPars.Size() == 0
				return null
			return GetTuple(itBox).ClassType
		}
		lazy := false
		while true
		{
			itr2 := Node.Down
			if itr2 == null break
			if itr2.GetValue() != "!" break
			itr2 = itr2.Right
			if itr2.GetValue() != "()"  break
			itr2 = itr2.Right
			lazy = itr2.GetValue() == "^"  or itr2.GetValue() == "&"
			break
		}
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
				types.PushFront(GTypeVoidP)
				someType = GetFuncType(types,null->{bool^},someType,false,isVARR)
				asN := someType->{TypeFunc^}
				return asN.GetLambda()
			}

			someType = GetFuncType(types,null->{bool^},someType,false,isVARR)
			for ptrSize someType = someType.GetPoint()
			return someType
		}
		
		if Node.Down.GetValue() == "~ind" and Node.Down.Right?.GetValue() == "." and Node.Down.Right.Right?.GetValue() == "{}"
		{
			SyntaxCompress(Node.Down.Right.Right,PriorityData)
			itName := ((Node.Down)->{ObjIndent^}).MyStr

			if itName == "Tuple"
			{
				itms  := new FuncInputBox()  ; $temp
				if Node.Down.Right.Right.Down[^].GetValue() != ","
				{
					isType := ParseType(it)
					if isType == null
					{
						return null
					}else{
						itms.itPars.Emplace(isType,false)
						
					}
				}
				asCt :=  GetTuple(itms)
				if asCt == null
					return null
				return asCt.ClassType
			}

			NodeName := GetItem(itName,Node)

			if NodeName == null return null
			if NodeName.Down.GetValue() == "!{}{...}"
			{
				asT := ((NodeName.Down)->{BoxClassTemplate^})
				box := new FuncInputBox() ; $temp
				
				iterR := Node.Down.Right.Right.Down
				
				while iterR != null
				{	
					if iterR.GetValue() != ","
					{
						isType := ParseType(iterR)
						if isType == null
						{
							val := TryCompute(iterR)
							if val != null box.itConsts.Push(val)
							else 
							{
								//Node.EmitError("can not create class\n")
								return null
							}
						}else{
							box.itConsts.Push(new ObjType(isType))
						}
					}
					iterR = iterR.Right
				}
				if box.itConsts.Size() == 0 return null

				//if Node.inhAttrs != null
				//	box.itAttrs[key] = Node.inhAttrs^[^key]

				if toAdd != null
				{
					for v3,k3 : toAdd^
					{
						box.itAttrs[k3] = v3
					}
				}

				if Node.Line != null
				for value,key : Node.Line.itAttrs
				{
					if key[0] == '#'
					{
						switch key
						{
							case "#outer_class"
								itrC := Node
								while itrC != null
								{
									if itrC is BoxClass
										break
									itrC = itrC.Up
								}
								if itrC != null
								{
									asCl := itrC->{BoxClass^}

									for v2,k2 : asCl.ItAttrs
									{
										box.itAttrs[k2] = v2
									}
								}
							case void
						}
					}else{
						box.itAttrs[key] = value ; $temp
					}
				}


				return asT.GetClass(box^)
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

FuncTypeTable := Queue.{TypeFunc^}[9973]

GetFuncTypeCount := !() -> int
{
	counter := 0
	maxSize := 0
	for i : 9973
	{
		itSi := FuncTypeTable[i].Size()
		counter += itSi
		if maxSize < itSi maxSize = itSi
		
	}
	printf("max hash size %i\n",maxSize)
	return counter
}

ComputeFuncHash := !(Queue.{Type^} typs,bool^ isRefs, Type^ retT, bool retIsRef) -> int
{
	res := 3

	for t,i : typs
	{
		if t != null res += t.ItHash
		if isRefs != null and isRefs[i] res = res xor_b 157
		res += (t->{void^}->{u64} >> 6) % 128
		res = res*3
	}
	if retT != null
	{
		res = res xor_b retT.ItHash
	}
	return res
}
RecomputeFuncHash := !(int h) -> int
{	
	cVal := h + ((h >> 10) and_b 0x0F) // + h div 251460
	return cVal->{u32} % 9973
}

ExchangeFuncType := !(TypeFunc^ FType,Type^ retType) -> TypeFunc^
{
	hashValue := FType.ItHash
	if FType.RetType != null hashValue = hashValue xor_b FType.RetType.ItHash
	if retType != null hashValue = hashValue xor_b retType.ItHash
	indVal := RecomputeFuncHash(hashValue)

	for itT : FuncTypeTable[indVal]
	{
		if itT.ItHash == hashValue and itT.IsVArgs == FType.IsVArgs
			and itT.ParsCount == FType.ParsCount
			and itT.RetType == retType
		{
			IsFound := true

			for FType.ParsCount
			{
				if FType.Pars[it] != itT.Pars[it] IsFound = false 
				if FType.ParsIsRef[it] != itT.ParsIsRef[it] IsFound = false 
			}
			if IsFound
			{
				return itT
			}
		}
	}
	newTypeFunc := new TypeFunc(FType,retType)
	FuncTypeTable[indVal].Push(newTypeFunc)
	return newTypeFunc
}

GetFuncType := !(Queue.{Type^} lin,bool^ IsRefArr,Type^ retType, bool retRef2, bool IsVArgs2) -> TypeFunc^
{

	ExtraArr := new bool[lin.Size()]
	for it : lin, i : 0
	{
		ExtraArr[i] = false
		if IsRefArr != null ExtraArr[i] = IsRefArr[i]
		if it != null
		{
			if it is TypeClass ExtraArr[i] = true
			if it is TypeArr  ExtraArr[i] = true
		}
	}

	hashValue := ComputeFuncHash(lin,ExtraArr,retType,retRef2)
	indVal := RecomputeFuncHash(hashValue)

	for itT : FuncTypeTable[indVal]
	{
		if itT.ItHash == hashValue and itT.IsVArgs == IsVArgs2 and retRef2  == itT.RetRef
		{
			IsFound := true
			if itT.ParsCount == lin.Size() //VArg?
			{
				for itR : lin, i : 0
				{
					if itT.Pars[i] != itR {
						IsFound = false
						break
					}

					if i < lin.Size()
						if itT.ParsIsRef[i] != ExtraArr[i] {
							IsFound = false
							break
						}
				}
			}else{
				IsFound = false
			}
			if itT.RetType != retType
			{
				IsFound = false
			}
			if IsFound return itT
		}
	}

	newTypeFunc := new TypeFunc(lin,ExtraArr,retType,retRef2,IsVArgs2)
	newTypeFunc.DoMeta()
	FuncTypeTable[indVal].Push(newTypeFunc)
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
	this := !(string name, Object^ itVal) -> void
	{
		ItType = null
		ItName = name
		Down = itVal.Clone()
		Down.SetUp(this&)
		firstRun = true
	}
	GetValue := virtual !() -> string
	{
		return ":=type"
	}
	firstRun := bool
	GetType := virtual !() -> Type^
	{
		if ItType != null return ItType
		if firstRun
		{
			firstRun = false
			ItType = ParseType(Down)
		}
		return ItType
	}
	
}

TypeStandart := class extend Type{
	IRName := string
	ItSize  := int
	ItAlign := int
	
	this := !(string Name,string mName,  int siz, int Aln) -> void
	{
		IRName = Name 
		ItSize = siz
		ItAlign = Aln
		ItHash = GetNewId()
		
		if DebugMode
		{
			metaId = GetNewId()
			itStr := "!" + metaId  + " = " + mName + "\n"
			DebugMetaData.Push(itStr)
		}

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
	GetGoodName := virtual !() -> string { return IRName }
	GetAlign := virtual !() -> int
	{
		return ItAlign
	}
	GetSize := virtual !() -> int
	{
		return ItSize
	}
}

TypePoint := class extend Type
{
	this := !(Type^ nBase) -> void
	{
		ItHash = nBase.ItHash*11 + 31
		Clean()
		Base = nBase
		//ItName = Base.GetName() + "*"
		if DebugMode
		{
			if nBase == null or nBase.metaId == 0
			{
			}else{
				metaId = GetNewId()
				itStr := "!"sbt << metaId << " = !DIDerivedType(tag:DW_TAG_pointer_type, baseType: !" << nBase.metaId << " ,size: " << archSize << ", align: " << archSize << ")"
				itStr << "; " << GetGoodName() << "\n"
				DebugMetaData.Push(itStr)
			}
		}
	}
	GetType := virtual !() -> string
	{
		return "point"
	}
	GetNewName := virtual !() -> string
	{
		return Base.GetName() + "*"
	}
	GetGoodName := virtual !() -> string { return Base.GetGoodName() + "^" }
	GetAlign := virtual !() -> int
	{
		return 8
	}
	GetSize := virtual !() -> int
	{
		return 8
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

	this := !(TypeFunc^ FType,Type^ itRetTyp) -> void
	{
		ItName = null
		ParsCount = FType.ParsCount

		ParsIsRef = new bool[ParsCount]
		for i : ParsCount ParsIsRef[i] = FType.ParsIsRef[i]

		Pars = null
		if ParsCount != 0 {
			Pars = new Type^[ParsCount]
			for ParsCount Pars[it] = FType.Pars[it]
		}
		RetRef = FType.RetRef
		RetType = FType.RetType
		IsVArgs = FType.IsVArgs

		ItHash = FType.ItHash
		if RetType != null ItHash = ItHash xor_b RetType.ItHash
		if itRetTyp != null ItHash = ItHash xor_b itRetTyp.ItHash
		RetType = itRetTyp

		//ItName = GetNewName()
		DoMeta()
	}
	this := !(Queue.{Type^} P,bool^ retsRef,Type^ retType, bool isRetRef, bool IsV) -> void
	{
		RetType = retType
		RetRef = isRetRef
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
		IsVArgs = IsV

		ItHash = ComputeFuncHash(P,ParsIsRef,retType,isRetRef)

		//ItName = GetNewName()
		DoMeta()
	}
	DoMeta := !() -> void
	{
		if DebugMode
		{
			if RetType == null
				return void
			for i : ParsCount
			{
				if Pars[i] == null return void
			}
			if true
			{
				metaId = GetNewId()
				subMeta := GetNewId()
				itStr := "!"sbt
				itStr <<  subMeta << " = !{"
				if RetType == GTypeVoid
				{
					itStr << "null"
				}else{
					itStr << "!" << RetType.metaId
				}
				for i : ParsCount
				{
					itStr << ",!" 
					if ParsIsRef[i]
					{
						asP := Pars[i].GetPoint()
						itStr << asP.metaId

					}else{
						itStr << Pars[i].metaId
					}
				}
		
				itStr << "}\n"
				itStr << "!" << metaId << " = !DISubroutineType(types: !" << subMeta << ")\n"
				DebugMetaData.Push(itStr.Str())
			}
		}
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
		for i : ParsCount
		{
			if Pars[i] == null return ""
		}
		if (RetType is TypeArr or RetType is TypeClass) and not RetRef
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
	GetGoodName := virtual !() -> string { return GetName() }
	GetSkobs := !() -> string
	{
		ToRet := ""sbt
		ToRet << "("
		
		if RetType != null
		{
			if RetType is TypeArr  or RetType is TypeClass
			{
				if not RetRef
				{
					ToRet << RetType.GetName() << "* "
					if ParsCount > 0 ToRet << " , "
				}
			}
		}


		for i : ParsCount
		{
			if i != 0 ToRet <<  " , "
			if ParsIsRef[i]	ToRet << Pars[i].GetPoint().GetName()
			else ToRet << Pars[i].GetName()
		}
		if IsVArgs
		{
			if ParsCount != 0 ToRet <<  " , "
			ToRet << "..."
		}
		ToRet << ")"
		return ToRet.Str()
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
		ItHash = asBase.ItHash*3 + 2
		if DebugMode and asBase.metaId != 0
		{
			metaId = GetNewId()
			itStr := "!" + metaId + " = !DIDerivedType(tag:DW_TAG_pointer_type, baseType: !" + asBase.metaId + " ,size: " + archSize + ", align: " + archSize + ")\n" 
			DebugMetaData.Push(itStr)
		}
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
	GetGoodName := virtual !() -> string { return GetName() }
	GetNewNamePre := virtual !() -> string
	{
		asB := Base->{TypeFunc^}

		ToRet := ""sbt

		IsCompl := false

		if not asB.RetRef
		{
			IsCompl = IsComplexType(asB.RetType)
		}

		if IsCompl {
			ToRet << "void"
		}else{
			ToRet << asB.RetType.GetName()
		}
		ToRet << "(i8*"

		if IsCompl or asB.ParsCount != 0 ToRet << ","

		if IsCompl{
			ToRet << asB.RetType.GetName() << "*"			
			if asB.ParsCount != 0 ToRet << ","
		}
		
		for i : asB.ParsCount
		{
			if i > 0 ToRet << ","
			ToRet << asB.Pars[i].GetName()
		}
		ToRet << ")*"
		return ToRet.Str()
	}
	GetAlign := virtual !() -> int
	{
		return 8
	}
	GetSize := virtual !() -> int
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
		ItHash = B.ItHash*27 + S
		Base = B
		Size = S
		ItName = GetNewName()
		if DebugMode and Base.metaId != 0
		{
			metaId = GetNewId()
			itStr := "!" + metaId + " = !DICompositeType(tag:DW_TAG_array_type, baseType: !" + Base.metaId + " ,size: " + archSize + ", align: " + archSize + ")\n" 
			DebugMetaData.Push(itStr)
		}
	}
	GetType := virtual !() -> string
	{
		return "arr"
	}
	GetNewName := virtual !() -> string
	{
		return "[" + Size + " x " + Base.GetName() + "]"
	}
	GetGoodName := virtual !() -> string { return Base.GetGoodName() + "[" + Size + "]" }
	GetAlign := virtual !() -> int
	{
		return Base.GetAlign()
	}
	GetSize := virtual !() -> int
	{
		return Base.GetSize()*Size
	}
}
TypeFatArr := class extend Type
{
	this := !(Type^ B) -> void
	{
		ItHash = B.ItHash*3 + 1
		Base = B
		if DebugMode and Base.metaId != 0
		{
			metaId = GetNewId()
			itStr := "!" + metaId + " = !DIDerivedType(tag:DW_TAG_pointer_type, baseType: !" + Base.metaId + " ,size: " + archSize + ", align: " + archSize + ")\n" 
			DebugMetaData.Push(itStr)
		}
	}
	GetType := virtual !() -> string
	{
		return "fatarr"
	}
	GetNewName := virtual !() -> string
	{
		return Base.GetName() + "*"
	}
	GetGoodName := virtual !() -> string { return Base.GetGoodName() + "[]" }
	GetAlign := virtual !() -> int
	{
		return 8
	}
	GetSize := virtual !() -> int
	{
		return 8
	}
}
TypeClass := class extend Type
{
	ToClass := BoxClass^
	this := !(BoxClass^ ToSet) -> void
	{
		ItHash = ToSet.ClassId
		ToClass = ToSet
		ItName = GetNewName()
		metaId = ToSet.ClassId
	}
	GetType := virtual !() -> string
	{
		return "class"
	}
	GetNewName := virtual !() -> string
	{
		return "%Class" + ToClass.ClassId
	}
	GetGoodName := virtual !() -> string { 
		st := ToClass.ClassName 
		if ToClass.ItAttrs.Size() != 0
		{
			st = st + " attrs: "
			for it,ind : ToClass.ItAttrs
				st = st + ind + " "
		}
		return st
	}
	
	itAlign := int
	itSize := int
	computedAlign := bool
	debTable := List.{Tuple.{char^,MemParam^,int,int}}
	ComputeAlignAndDeb := !() -> void
	{
		if computedAlign return void
		computedAlign = true
		itAlign = 1
		
		nowSize := 0
		if not ToClass.vTable.Empty()
			nowSize = 8
		for it : ToClass.Params
		{
			sAlign := it.ResultType.GetAlign()
			//assert(sAlign <= 16 and sAlign > 0)
			alignMask :=  ![0,0,1,0,3,0,0,0,7,0,0,0,0,0,0,0,15][sAlign]
			sSize := it.ResultType.GetSize()

			if not ToClass.IsPacked and (nowSize and_b alignMask) != 0
			{
				nowSize = nowSize + alignMask
				nowSize = nowSize - (nowSize and_b alignMask)
			}
			debTable.Emplace(it.ItName,it,nowSize,sSize)
			nowSize += sSize
			if sAlign > itAlign
				itAlign = sAlign
		}
		itSize = nowSize
		if ToClass.IsPacked
			itAlign = 1
	}

	GetAlign := virtual !() -> int
	{
		ComputeAlignAndDeb()
		return itAlign
	}
	GetSize := virtual !() -> int
	{
		ComputeAlignAndDeb()
		return itSize
	}
}

GetType := !(string Name) -> Type^
{
	for i : DefsTable->len
	{
		it := ref DefsTable[i]
		if it.ItName == Name return it.ItType
	}
	return null
}

CreateStandartTypes := !() -> void
{	
	//u
	TypeTable[0] = new TypeStandart("i8" ,"!DIBasicType(name: \"byte\", size: 8, align: 8,encoding: DW_ATE_unsigned_char)",1,1)	
	TypeTable[1] = new TypeStandart("i16","!DIBasicType(name: \"ushort\", size: 16, align: 16,encoding: DW_ATE_unsigned)",2,2)	
	TypeTable[2] = new TypeStandart("i32","!DIBasicType(name: \"uint\", size: 32, align: 32,encoding: DW_ATE_unsigned)",4,4)	
	TypeTable[3] = new TypeStandart("i64","!DIBasicType(name: \"ulong\", size: 64, align: 64,encoding: DW_ATE_unsigned)",8,8)

	//s
	TypeTable[4] = new TypeStandart("i8","!DIBasicType(name: \"char\", size: 8, align: 8,encoding: DW_ATE_signed_char)",1,1)	
	TypeTable[5] = new TypeStandart("i16","!DIBasicType(name: \"short\", size: 16, align: 16,encoding: DW_ATE_signed)",2,2)	
	TypeTable[6] = new TypeStandart("i32","!DIBasicType(name: \"int\", size: 32, align: 32,encoding: DW_ATE_signed)",4,4)	
	TypeTable[7] = new TypeStandart("i64","!DIBasicType(name: \"long\", size: 64, align: 64,encoding: DW_ATE_signed)",8,8)

	TypeTable[8] = new TypeStandart("i1","!DIBasicType(name: \"byte\", size: 8, align: 1,encoding: DW_ATE_boolean)",1,1)	// bool

	TypeTable[9] = new TypeStandart("float","!DIBasicType(name: \"float\", size: 32, align: 32,encoding: DW_ATE_float)",4,4)
	TypeTable[10] = new TypeStandart("double","!DIBasicType(name: \"double\", size: 64, align: 64,encoding: DW_ATE_float)",8,8)
	TypeTable[11] = new TypeStandart("void","!DIBasicType(name: \"void\", size: 32, align: 32,encoding: DW_ATE_signed)",0,0)

	TypeTable[12] = TypeTable[0].GetPoint() // char*

	VoidPType = TypeTable[11].GetPoint() //new TypePointVoidP(TypeTable[11]) // void*
	VoidPType.ItName = "i8*"

	//TypeTable[16] = new TypePointVoidFatP(TypeTable[11])
	//TypeTable[11].AsFatArr = TypeTable[16]
	voidFatArr := TypeTable[11].GetFatArray()
	voidFatArr.ItName = "i8*"

	GlobalStrs.Push("%RangeTypeInt = type {i32,i32}\n")
	GlobalStrs.Push("%RangeTypeFloat = type {float,float}\n")

	TypeTable[13] = new TypeStandart("%RangeTypeInt","!DIBasicType(name: \"range\", size: 64, align: 64,encoding: DW_ATE_unsigned)",8,8)
	TypeTable[13].Base = TypeTable[6]
	TypeTable[14] = new TypeStandart("%RangeTypeFloat","!DIBasicType(name: \"rangef\", size: 32, align: 64,encoding: DW_ATE_float)",8,8)
	TypeTable[14].Base = TypeTable[9]


	TypeTable[15] = new TypeStandart("%OpaqType","!DIBasicType(name: \"opaque\", size: 8, align: 8,encoding: DW_ATE_unsigned)",0,0)

	TypeTable[17] = new TypeStandart("%Vec4f","!DIBasicType(name: \"vec4f\", size: 32, align: 128,encoding: DW_ATE_float)",16,16)
	TypeTable[18] = new TypeStandart("%Vec4f","!DIBasicType(name: \"vec4f\", size: 32, align: 128,encoding: DW_ATE_float)",16,16)

	TypeTable[19] = new TypeStandart("%Vec3f","!DIBasicType(name: \"vec3f\", size: 24, align: 128,encoding: DW_ATE_float)",12,16)
	TypeTable[20] = new TypeStandart("%Vec2f","!DIBasicType(name: \"vec2f\", size: 16, align: 128,encoding: DW_ATE_float)",8,16)

	TypeTable[21] = new TypeStandart("half","!DIBasicType(name: \"half\", size: 16, align: 16,encoding: DW_ATE_float)",2,2)

	GTypeBool =   TypeTable[8]
	GTypeHalf =   TypeTable[21]
	GTypeFloat =  TypeTable[9]
	GTypeDouble = TypeTable[10]
	GTypeInt =    TypeTable[6]
	GTypeU64 =    TypeTable[3]
	GTypeRange =  TypeTable[13]
	GTypeVoid =   TypeTable[11]
	GTypeString = TypeTable[12]
	GTypeVoidP =  GTypeVoid.GetPoint()
	GTypeVec4f = TypeTable[17]
	GTypeQuantf = TypeTable[18]
	GTypeVec3f = TypeTable[19]
	GTypeVec2f = TypeTable[20]


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

	DefsTable[22] = new TypeDef("size_t",TypeTable[3])
	DefsTable[23] = new TypeDef("half",TypeTable[21])

	DefsTable[24] = new TypeDef("vec3f",GTypeVec3f)
	DefsTable[25] = new TypeDef("vec2f",GTypeVec2f)
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

TypeContainVTable := !(Type^ toTst) -> bool
{
	if toTst is TypeClass
	{
		asCl := toTst->{TypeClass^}.ToClass
		if asCl.ContainVirtual
			return true
		for itP : asCl.Params
		{
			if TypeContainVTable(itP.ResultType)
				return true
		}
	}
	if toTst is TypeArr
	{
		return TypeContainVTable(toTst.Base)
	}

	return false
}
TypeFight := !(Type^ A,Type^ B) -> Type^
{
	if A == B return A

	chA := TypeCmp(A,B)
	chB := TypeCmp(B,A)
	if chA != 255 and chB != 255
	{
		if chA < chB
		{
			return B
		}
		return A
	}
	return null
}

debRefs := AVLMap.{Type^,int}
GetDebugRef := !(Type^ itTyp) -> int
{
	inMap := debRefs.TryFind(itTyp)
	if inMap != null{
		return inMap^
	}
	newId :=  GetNewId()
	debRefs[itTyp] = newId
	return newId
}
PrintDebRefs := !(sfile f) -> void
{
	for it,ind : debRefs
	{
		f << "!" << it << " = !DIDerivedType(tag: DW_TAG_reference_type, baseType: !" << ind.metaId <<", size: 64)"
	}
}
