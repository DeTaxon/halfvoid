
JITMode := false

jitCTX := void^
initJIT := !() -> void
{
	libjitinit()
	jitCTX = jit_context_create()
}

JITCFunc := void^
JITFunc := List.{void^} ; $keep
JITResultStack := List.{void^} ; $keep

JITCachedTypes := AVLMap.{Type^,void^}
GetJITType := !(Type^ toChange) -> void^
{
	if toChange in JITCachedTypes
		return JITCachedTypes[toChange]
	
	newType := void^
	if toChange is TypeStandart
	{
		switch toChange
		{
			case GTypeInt newType = jit_type_int
			case GTypeVoid newType = jit_type_void
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
				prs.Push(GetJITType(asFunc.Pars[i]))
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
	assert(newType != null)
	JITCachedTypes[toChange] = newType

	return newType
}
