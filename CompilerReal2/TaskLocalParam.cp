TaskLocalParam := class extend GlobalParam
{ 
	DoTheWork := virtual !(int pri) -> void  {}
	PrintGlobal := virtual !(TIOStream f) -> void {}
	GetTaskFieldId := !() -> void
	{
		taskFieldId = GetTaskLocalId(ResultType)
	}
	PrepareMainPtr := virtual !(TIOStream f,int newInd,int debId) -> void {
		if jitMode
		{
			f << "%TaskGetPtr" << newInd << " = inttoptr i64 " << internalGetTaskPtr->{void^}->{size_t} << " to i8*()*\n"
			f << "%TaskTPre" << newInd << " = call i8* %TaskGetPtr"<< newInd <<"()\n"
			f << "%TaskValuePtrPre" << newInd << " = bitcast i8* %TaskTPre" << newInd << " to %TaskStruct*\n"
			f << "%TaskValuePtr" << newInd << " = getelementptr %TaskStruct, %TaskStruct* %TaskValuePtrPre" << newInd << ", i32 0, i32 " << taskFieldId << "\n"
			return void
		}

		if EnableGSTask
		{
		f << "%TaskPre" << newInd << " = addrspacecast %TaskStruct addrspace(256)* @CurrentTaskStruct to %TaskStruct*\n"
		f << "%TaskValuePtr" << newInd << " = getelementptr %TaskStruct , %TaskStruct* %TaskPre"<< newInd <<" , i32 0, i32 " << taskFieldId << "\n"
		}else{
		f << "%TaskTPre" << newInd << " = load i8*, i8** " << gTaskPtr.GetPointName(0) << "\n"
		f << "%TaskValuePtrPre" << newInd << " = bitcast i8* %TaskTPre" << newInd << " to %TaskStruct*\n"
		f << "%TaskValuePtr" << newInd << " = getelementptr %TaskStruct, %TaskStruct* %TaskValuePtrPre" << newInd << ", i32 0, i32 " << taskFieldId << "\n"
		}
		
	}
	GetMainPtr := virtual !(int newId) -> char^ { return "%TaskValuePtr"sbt + newId}
}

internalGetTaskPtr := !() -> void^
{
	return _TaskPtrGet()
}
