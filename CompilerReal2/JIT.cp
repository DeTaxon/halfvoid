
jitMode := false

internalMemset := BoxFunc^

JITPreInit := !() -> void
{
	if not jitMode
		return void
}

JITInit := !() -> void
{
	box := new FuncInputBox ; $temp
	box.itPars.Emplace(GTypeVoidP,false)
	box.itPars.Emplace(GTypeSizeT,false)

	internalMemset = FindFunc("internalGCMemClear",null,box^,false)
	assert(internalMemset != null)

	if not jitMode
		return void
	
}
