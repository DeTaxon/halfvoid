
jitMode := false

internalMemset := BoxFunc^

passJITFuncs := AVLMap.{char^,Tuple.{Type^,MemParam^}}

JITPreInit := !() -> void
{
	if not jitMode
		return void

	sleepType := GetFuncType(![GTypeDouble],GTypeVoid)
	sleepTypePtr := sleepType.GetPoint()
	sleepPar := new GlobalParam(sleepTypePtr,new ObjPointer(TSleep->{void^},sleepTypePtr))
	passJITFuncs["TSleep"] = !{GetFuncType(![GTypeDouble],GTypeVoid),sleepPar}
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
JITPrint := !(TIOStream f) -> void
{
	if not jitMode
		return void
	for it : passJITFuncs
	{
		it.1.PrintGlobal(f)
	}
}
