CmpFuncInputOnly := !(Type^ f1, Type^ f2) -> bool
{
	if not f1 is TypeFunc return false
	if not f2 is TypeFunc return false

	F1 := f1->{TypeFunc^}
	F2 := f2->{TypeFunc^}

	if F1.ParsCount != F2.ParsCount return false
	if F1.IsVArgs != F2.IsVArgs return false

	for i : F1.ParsCount
	{
		if F1.Pars[i] != F2.Pars[i] return false
		if F1.ParsIsRef[i] != F2.ParsIsRef[i] return false
	}

	return true
}


TypeFuncRaw := class
{
	RetType := Type^
	Pars := Type^[]
	isVArgs := bool
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

	GetRawFunc := !() -> TypeFuncRaw^
	{
		preRet := new TypeFuncRaw ; $temp
		pc := ParsCount
		isComplx := false
		if ((RetType is TypeArr) or (RetType is TypeClass)) and not RetRef
			isComplx = true
		if isComplx 
		{
			pc += 1
			preRet.RetType = GTypeVoid
		}else{
			if RetRef
			{
				preRet.RetType = RetType.GetPoint()
			}else{
				preRet.RetType = RetType
			}
		}
		preRet.Pars = new Type^[pc] ; $temp
		i := 0
		if isComplx
		{
			preRet.Pars[i++] = RetType.GetPoint()
		}
		for j : ParsCount
		{
			if ParsIsRef[j]
			{
				preRet.Pars[i] = Pars[j].GetPoint()
			}else{
				preRet.Pars[i] = Pars[j]
			}
			i += 1
		}
		return preRet
	}
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
	this := !(Type^^ typs,bool^ parsIsRef,int parsCount,Type^ retType, bool isRetRef, bool IsV) -> void
	{
		RetType = retType
		RetRef = isRetRef
		ParsCount = parsCount

		if parsIsRef != null
		{
			ParsIsRef = new bool[ParsCount]
			for i : ParsCount ParsIsRef[i] = parsIsRef[i]
		}else{
			if ParsCount != 0
			{
				ParsIsRef = new bool[ParsCount]
				for i : ParsCount ParsIsRef[i] = 0
			}
		}
		Pars = null
		if ParsCount != 0 {
			Pars = new Type^[parsCount]
			for i : parsCount
			{
				Pars[i] = typs[i]
			}
		}
		IsVArgs = IsV

		ItHash = ComputeFuncHash(typs,ParsIsRef,parsCount,retType,isRetRef)

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
			preRes := ""sbt + "void" + GetSkobs() 
			return preRes.Str()
		}
		if RetRef {
			preRes := ""sbt + RetType.GetName() + "*" + GetSkobs()
			return preRes.Str()
		}
		preRes2 := ""sbt + RetType.GetName() + GetSkobs() 
		return preRes2.Str()
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
	PrintSkobs := virtual !(TIOStream f) -> void
	{
		f << GetSkobs()
	}
	PrintType := virtual !(TIOStream f) -> void
	{
		f << GetName()
	}
	GetAlign := virtual !() -> int
	{
		return 8
	}
}