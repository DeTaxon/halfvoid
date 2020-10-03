

gTaskPtr := GlobalParam^



TaskInit := !() -> void
{
	gTaskPtr = new GlobalParam(GTypeVoidP,null)
	gTaskPtr.IsThreadLocal = true
	GlobalDataBuiltins["_TaskPtr"] = gTaskPtr
	
	taskSize := new BuiltInFuncZero("_getTaskStructSize",GTypeU64,false,"#0Pre = getelementptr %TaskStruct, %TaskStruct* null,i32 1\n"sbt
							 +"#0 = ptrtoint %TaskStruct* #0Pre to i64\n")
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
}



