
jitMode := false

internalMemset := BoxFunc^

passJITFuncs := AVLMap.{char^,Tuple.{Type^,MemParam^}}

JITPreInit := !() -> void
{
	if not jitMode
		return void

	addJITFunction("TSleep",TSleep->{void^},![GTypeDouble],GTypeVoid)
	addJITFunction("TExpectWorkers",TExpectWorkers->{void^},![GTypeInt],GTypeVoid)
	
	lType := GetFuncType(![GTypeVoidP],GTypeVoid).GetLambda()
	addJITFunction("TSpawnTask",TSpawnTask->{void^},![lType],GTypeVoid)
}

addJITFunction := !(char^ name ,void^ fPoint, Type^[@ArrSize] inps,Type^ outType) -> void
{
	sleepType := GetFuncType(inps,outType)
	sleepTypePtr := sleepType.GetPoint()
	sleepPar := new GlobalParam(sleepTypePtr,new ObjPointer(fPoint->{void^},sleepTypePtr))
	passJITFuncs[name] = !{sleepType,sleepPar}
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
