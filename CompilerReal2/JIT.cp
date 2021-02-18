
JITMode := false

jitLib := Library
initJIT := !() -> void
{
	jitLib.Open("libjit.so")
	//libjitinit()
}

JITCachedTypes := AVLMap.{Type^,void^}
GetJITType := !(Type^ toChange) -> void^
{
	if toChange in JITCachedTypes
		return JITCachedTypes[toChange]
	
	if toChange is TypeStandart
	{
		newType := void^
		switch toChange
		{
			case GTypeInt newType = jit_type_int
			case GTypeVoid newType = jit_type_void
		}
		assert(newType != null)
		JITCachedTypes[toChange] = newType
		return newType
	}
	if toChange is TypePoint
	{
		
	}
}
