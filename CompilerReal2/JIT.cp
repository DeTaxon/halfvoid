
JITMode := false

jitCTX := void^
initJIT := !() -> void
{
	libjitinit()
	jitCTX = jit_context_create()

	jit_label_undefined = null

	intPtr := jit_label_undefined&->{size_t^}
	intPtr^ -= 1
	intPtr^ = intPtr^ >> 32
}

jit_label_undefined := void^


JITCFunc := void^
JITCFuncH := Object^
JITFunc := List.{Tuple.{void^,Object^}} ; $keep
JITResultStack := List.{void^} ; $keep

JITCachedTypes := AVLMap.{Type^,void^}
GetJITType := !(Type^ toChange) -> void^
{
	if toChange in JITCachedTypes
		return JITCachedTypes[toChange]
	
	newType := void^
	if toChange is TypeStandart
	{
		found := false
		for i : 8
		{
			if TypeTable[i] == toChange
			{
				newType = jit_type_copy(jit_type_int)
				jit_type_set_size_and_alignment(newType,TypeTable[i].GetSize(),TypeTable[i].GetAlign())
				found = true
			}
		}
		if not found
		{
			switch toChange
			{
				case GTypeBool 
				newType = jit_type_copy(jit_type_int)
				jit_type_set_size_and_alignment(newType,1,1)
				case GTypeVoid newType = jit_type_void
			}
		}
	}
	if toChange is TypePoint
	{
		fromType := GetJITType(toChange.Base)
		newType = jit_type_create_pointer(fromType,1)
	}
	if toChange is TypeFunc
	{
		asFunc := toChange->{TypeFunc^}

		rType := asFunc.RetType
		if asFunc.RetRef rType = rType.GetPoint()

		itCall := jit_abi_cdecl
		if asFunc.IsVArgs itCall = jit_abi_vararg

		prs := List.{void^}() ; $temp

		if not asFunc.RetRef and (asFunc.RetType? is TypeArr or asFunc.RetType? is TypeClass)
		{
			prs.Push(GetJITType(asFunc.RetType.GetPoint()))
			rType = GTypeVoid
		}
		for i : asFunc.ParsCount
		{
			
			if asFunc.ParsIsRef[i]
			{
				prs.Push(GetJITType(asFunc.Pars[i].GetPoint()))
			}else{
				pType := GetJITType(asFunc.Pars[i])
				assert(pType != null)
				prs.Push(pType)
			}
		}

		if prs.Size() == 0
		{
			newType = jit_type_create_signature(itCall,GetJITType(rType),null,0,1)
		}else{
			asArr := prs.ToArray()
			newType = jit_type_create_signature(itCall,GetJITType(rType),asArr,prs.Size(),1)
		}

	}
	if toChange is TypeClass
	{
		//JIT packed_class better_yodlers? virtual
		asCl := toChange->{TypeClass^}.ToClass
		if asCl.Params.Size() == 0
		{
			newType = GetJITType(GTypeChar)
		}else{
			flds := new void^[asCl.Params.Size()] ; $temp
			for it,i : asCl.Params
			{
				flds[i] = GetJITType(it.GetType())
			}
			newType = jit_type_create_struct(flds,asCl.Params.Size(),1)
		}
	}
	assert(newType != null)
	JITCachedTypes[toChange] = newType

	return newType
}


JITSpecialFunc := !(NaturalCall^ toCl,void^^ res) -> bool
{
	asFunc := clibModule.GetPtrFunc(toCl.ToCall.FuncName)
	if asFunc != null
	{
		dwns := List.{void^}() ; $temp
		for it : toCl.Down
		{
			jtVal := it.DoJIT()
			assert(jtVal != null)
			dwns.Push(jtVal)
		}
		lst := dwns.ToArray() ; $temp
		fType := toCl.ToCall.MyFuncType
		fJITType := void^
		if fType.IsVArgs and dwns.Size() != fType.ParsCount
		{
			rf := fType.GetRawFunc()
			asArr := new void^[dwns.Size()]
			for i : dwns.Size(), dwn : toCl.Down
			{
				if i < rf.Pars->len
				{
					asArr[i] = GetJITType(rf.Pars[i])
				}else{
					asArr[i] = GetJITType(dwn.GetType())
				}
			}
			fJITType = jit_type_create_signature(jit_abi_vararg,GetJITType(rf.RetType),asArr,dwns.Size(),1) //TODOJIT: memleak
		}else{
			fJITType = GetJITType(fType)
		}
		res^ = jit_insn_call_native(JITCFunc,toCl.ToCall.FuncName,asFunc,fJITType,lst,dwns.Size(),JIT_CALL_NOTHROW)
		return true
	}
	return false
}