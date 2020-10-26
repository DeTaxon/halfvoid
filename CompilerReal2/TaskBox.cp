

gTaskPtr := GlobalParam^

//EnableGSTask

TaskInit := !() -> void
{
	gTaskPtr = new GlobalParam(GTypeVoidP,null)
	gTaskPtr.IsThreadLocal = true
	//GlobalDataBuiltins["_TaskPtr"] = gTaskPtr
	
	taskSize := new BuiltInFuncZero("_getTaskStructSize",GTypeU64,false,"#0Pre = getelementptr %TaskStruct, %TaskStruct* null,i32 1\n"sbt
							 +"#0 = ptrtoint %TaskStruct* #0Pre to i64\n")

	AddBuiltInFunc(new BuiltInFuncUno("_TaskPtrSet",GTypeVoidP,false,GTypeVoid,false,"store i8* #1 , i8** "sbt + gTaskPtr.GetPointName(0) + "\n"))
	AddBuiltInFunc(new BuiltInFuncZero("_TaskPtrReset",GTypeVoid,false,
		"%ItStruct## = bitcast %TaskStruct* @DefTaskStruct to i8*\n"sbt +
		"store i8* %ItStruct## , i8** "sbt + gTaskPtr.GetPointName(0) + "\n"))
	AddBuiltInFunc(taskSize)

}

taskParams := List.{Type^}

GetTaskLocalId := !(Type^ crtTyp) -> int
{
	oldSize := taskParams.Size()
	taskParams.Push(crtTyp)
	return oldSize
}
TaskPrint := !(sfile f) -> void
{
	f << "%TaskStruct = type{"
	for it,i : taskParams
	{
		if i != 0 f << ","
		f << it.GetName()
	}
	f << "}\n"
	f << "@DefTaskStruct = thread_local global %TaskStruct zeroinitializer\n"

	if EnableGSTask
	{
		f << "@ThisTaskStruct = addrspace(256) global %TaskStruct zeroinitializer\n"
	}
	gTaskPtr.PrintGlobal(f)
}
TaskPrintInit := !(sfile f) -> void
{
	f << "%ItStructObj = bitcast %TaskStruct* @DefTaskStruct to i8*\n"
	f << "store i8* %ItStructObj , i8** "sbt + gTaskPtr.GetPointName(0) + "\n"
}



