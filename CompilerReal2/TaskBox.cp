

gTaskPtr := GlobalParam^

//EnableGSTask

TaskInit := !() -> void
{
	gTaskPtr = new GlobalParam(GTypeVoidP,null)
	gTaskPtr.IsThreadLocal = true
	//GlobalDataBuiltins["_TaskPtr"] = gTaskPtr
	
	taskSize := new BuiltInFuncZero("_getTaskStructSize",GTypeU64,false,"#0Pre = getelementptr %TaskStruct, %TaskStruct* null,i32 1\n"sbt
							 +"#0 = ptrtoint %TaskStruct* #0Pre to i64\n")
	AddBuiltInFunc(taskSize)

	if EnableGSTask
	{
		AddBuiltInFunc(new BuiltInFuncUno("_TaskPtrSet",GTypeVoidP,false,GTypeVoid,false,
			"%ItStruct## = ptrtoint i8* #1 to i64\n"sbt +
			"call void @llvm.x86.wrgsbase.64(i64 %ItStruct##)\n"))
		AddBuiltInFunc(new BuiltInFuncZero("_TaskPtrReset",GTypeVoid,false,
			"%ItStruct## = ptrtoint %TaskStruct* @DefTaskStruct to i64\n"sbt +
			"call void @llvm.x86.wrgsbase.64(i64 %ItStruct##)\n"))
	}else{
		AddBuiltInFunc(new BuiltInFuncUno("_TaskPtrSet",GTypeVoidP,false,GTypeVoid,false,"store i8* #1 , i8** "sbt + gTaskPtr.GetPointName(0) + "\n"))
		AddBuiltInFunc(new BuiltInFuncZero("_TaskPtrReset",GTypeVoid,false,
			"%ItStruct## = bitcast %TaskStruct* @DefTaskStruct to i8*\n"sbt +
			"store i8* %ItStruct## , i8** "sbt + gTaskPtr.GetPointName(0) + "\n"))
		AddBuiltInFunc(new BuiltInFuncZero("_TaskPtrGet",GTypeVoidP,true,"#0 = getelementptr i8*, i8** "sbt + gTaskPtr.GetPointName(0) + ", i32 0\n"))

	}
	AddBuiltInFunc(new BuiltInFuncUno("_taskInitMem",GTypeVoidP,false,GTypeVoid,false,
							 "call void @taskInitFunc(i8* #1)\n"))

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
	if EnableGSTask
	{
		f << "declare void @llvm.x86.wrgsbase.64(i64 %a)\n" 
	}

	f << "%TaskStruct = type{"
	for it,i : taskParams
	{
		if i != 0 f << ","
		f << it.GetName()
	}
	f << "}\n"

	if EnableGSTask
	{
		f << "@CurrentTaskStruct = addrspace(256) global %TaskStruct zeroinitializer\n"
		f << "@DefTaskStruct = thread_local global %TaskStruct zeroinitializer\n"
	}else{
		f << "@DefTaskStruct = thread_local global %TaskStruct zeroinitializer\n"
		gTaskPtr.PrintGlobal(f)
	}
}
TaskPrint2 := !(sfile f) -> void
{
	f << "define void @taskInitFunc(i8* %ptr)\n"
	f << "{\n"
	f << "%TaskPtr = bitcast i8* %ptr to %TaskStruct*\n"
	for it,i : taskParams
	{
		if it is TypeClass //TODO; if array of array ... of classes
		{
			clId := it->{TypeClass^}.ToClass.ClassId
			f << "%obj"<<i<<" = getelementptr %TaskStruct, %TaskStruct* %TaskPtr, i32 0, i32 " << i << "\n"
			f << "call void @ClassExtraConstructor" << clId
			f << "(" << it.GetName() << "* %obj" << i << ")\n"
		}
	}
	f << "ret void\n}\n"
}
TaskPrintInit := !(sfile f) -> void
{
	if EnableGSTask
	{
	f << "%ItStructObj = ptrtoint %TaskStruct* @DefTaskStruct to i64\n"
	f << "call void @llvm.x86.wrgsbase.64(i64 %ItStructObj)\n"
	}else{
	f << "%ItStructObj = bitcast %TaskStruct* @DefTaskStruct to i8*\n"
	f << "store i8* %ItStructObj , i8** "sbt + gTaskPtr.GetPointName(0) + "\n"
	}
}



