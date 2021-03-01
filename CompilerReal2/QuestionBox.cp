ControlFlowBox := class extend Object
{
	GetValue := virtual !() -> string
	{
		return "?or??"
	}
	PrintInBlock := virtual !(sfile f) -> void { PrintPre(f)}
}
QuestionBox := class extend ControlFlowBox
{	
	itId := int
	paramObject := FuncParam^
	replObject := Object^
	forceToBool := bool
	jmpName := char^
	isSimpleCheck := bool
	passValue := bool
	this := !(Object^ tmpObj,bool fTB) -> void
	{
		itId = GetNewId()
		replObject = tmpObj
		forceToBool = fTB
		jmpName = StrCopy("OnBad"sbt + itId)
	}
	IsRef := virtual !() -> bool
	{
		return Down.Right.IsRef()
	}
	PrintPointPre := virtual !(sfile f) -> void
	{
		dwnType := Down.GetType()
		DR := Down.Right

		Down.PrintPointPre(f)
		f << "br label %Start" << itId << "\n"
		f << "Start" << itId << ":\n"
		f << "%QTempObject" << itId << " = getelementptr "<< dwnType.GetName() << " , " Down.PrintPointUse(f) f << " , i32 0\n"
		f << "%CmpRes" << itId << " = icmp ne " 
		Down.PrintPointUse(f)
		f << " , null\n"
		f << "br i1 %CmpRes" << itId << ", label %OnGood" << itId <<", label %"<< jmpName << "\n"
		f << "OnGood" << itId << ":\n"
		if DR is BoxSwitch or DR is BoxReturn
		{
			DR.PrintInBlock(f)
		}else{
			DR.PrintPre(f)
		}
		f << "br label %DownRes" << itId << "\n"
		f << "DownRes" << itId << ":\n"
		f << "br label %OnBad" << itId << "\n"
		f << "OnBad" << itId << ":\n"
		
		if forceToBool
		{
			f << "%Res"<< itId <<"  = phi i1 [false,%Start" << itId << "] , [" << DR.GetName() << ",%DownRes" << itId << "]\n"
		}
	}
	PrintPre := virtual !(sfile f) -> void
	{
		dwnType := Down.GetType()

		if dwnType is TypePoint or dwnType is TypeFatArr
		{
			DR := Down.Right

			Down.PrintPre(f)
			f << "br label %Start" << itId << "\n"
			f << "Start" << itId << ":\n"
			if dwnType == GTypeVoidP
			{
				f << "%QTempObject" << itId << " = getelementptr i8* , " Down.PrintUse(f) f << " , i32 0\n"
			}else{
				f << "%QTempObject" << itId << " = getelementptr "<< dwnType.Base.GetName() << " , " Down.PrintUse(f) f << " , i32 0\n"
			}
			f << "%CmpRes" << itId << " = icmp ne " 
			Down.PrintUse(f)
			f << " , null\n"
			f << "br i1 %CmpRes" << itId << ", label %OnGood" << itId <<", label %"<< jmpName << "\n"
			f << "OnGood" << itId << ":\n"
			if DR is BoxSwitch or DR is BoxReturn
			{
				DR.PrintInBlock(f)
			}else{
				DR.PrintPre(f)
			}
			f << "br label %DownRes" << itId << "\n"
			f << "DownRes" << itId << ":\n"
			f << "br label %OnBad" << itId << "\n"
			f << "OnBad" << itId << ":\n"
			
			if forceToBool
			{
				f << "%Res"<< itId <<"  = phi i1 [false,%Start" << itId << "] , [" << DR.GetName() << ",%DownRes" << itId << "]\n"
			}
		}else{
			PrintPointPre(f)
		}
	}
	StepTwo := bool
	DoTheWork := virtual !(int pri) -> void
	{
		if visitedWork[pri] return void
		if pri == State_Start
		{
			WorkBag.Push(this&,State_GetUse)
			WorkBag.Push(Down,State_Start)
			visitedWork << pri
		}
		if pri == State_GetUse
		{
			if not StepTwo
			{
				dwnType := Down.GetType()
				if dwnType is TypeFatArr or dwnType is TypePoint
				{	
					WorkBag.Push(this&,State_GetUse)
					WorkBag.Push(Down.Right,State_Start)
					paramObject = new FuncParam("QTempObject"sbt + itId,dwnType,false)
					ReplaceNode(replObject,new ParamNaturalCall("_",paramObject))
					StepTwo = true
				}else{
					if not Down.IsRef()
					{
						Down.Print(0)
						EmitError("question type is not a pointer, its "sbt + dwnType.GetType() + " \n")
					}else{
						WorkBag.Push(this&,State_GetUse)
						WorkBag.Push(Down.Right,State_Start)
						paramObject = new FuncParam("QTempObject"sbt + itId,dwnType,true)
						ReplaceNode(replObject,new ParamNaturalCall("_",paramObject))
						StepTwo = true
					}
				}
			}else{
				if Down.Right.GetType() != GTypeBool
				{	
					if forceToBool
					{
						if TypeCmp(Down.Right.GetType(),GTypeBool) != 255
							BoxExc(Down.Right,GTypeBool,false)
						else EmitError("can not use non bool type")
					}
				}
				visitedWork << pri
			}
		}
	}
	GetType := virtual !() -> Type^ { 
		if passValue
			return Down.Right.GetType()
		return GTypeBool 
	}
	PrintUse := virtual !(sfile f) -> void { 
		if passValue {
			Down.Right.PrintUse(f)
		}else{
			f << "i1 %Res" << itId 
		}
	}
	GetName := virtual !() -> char^ {
		if passValue return Down.Right.GetName()
		return "%Res"sbt + itId 
	}
}

FlowOrOrAnd := class extend ControlFlowBox
{
	itFlowId := int
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			WorkBag.Push(this&,State_GetUse)
			WorkBag.Push(Down,State_Start)
			WorkBag.Push(Down.Right,State_Start)
		}
		if pri == State_GetUse
		{
			if Down.GetType() != GTypeBool
			{	
				if TypeCmp(Down.GetType(),GTypeBool) != 255 {
					BoxExc(Down,GTypeBool,false)
				}else {
					EmitError("can not use non bool type")
					return void
				}
			}
			if Down.Right.GetType() != GTypeBool
			{
				if TypeCmp(Down.Right.GetType(),GTypeBool) != 255
					BoxExc(Down,GTypeBool,false)
				else EmitError("can not use non bool type")
			}
		}
	}
	GetType := virtual !() -> Type^ { return GTypeBool }
	PrintUse := virtual !(sfile f) -> void { f << "i1 %Res" << itFlowId }
	GetName := virtual !() -> char^ {
		return "%Res"sbt + itFlowId 
	}
	
}

FlowOr := class extend FlowOrOrAnd
{	
	this := !() -> void
	{
		itFlowId = GetNewId()
	}
	PrintPre := virtual !(sfile f) -> void
	{
		Down.PrintPre(f)
		f << "br label %Start" << itFlowId << "\n"
		f << "Start" << itFlowId << ":\n"
		f << "br i1 " << Down.GetName() << ", label %OnGood" << itFlowId << " , label %OnBad" << itFlowId << "\n"
		f << "OnBad" << itFlowId << ":\n"
		Down.Right.PrintPre(f)
		f << "br label %BadFlow" << itFlowId << "\n"
		f << "BadFlow" << itFlowId << ":\n"
		//f << "%PreRes" << itFlowId << " = or i1 0, " << Down.Right.GetName() << "\n"
		f << "br label %OnGood" << itFlowId << "\n"
		f << "OnGood" << itFlowId << ":\n"
		f << "%Res" << itFlowId << " = phi i1 [1,%Start" << itFlowId << "] , [" <<Down.Right.GetName() << " , %BadFlow" << itFlowId << "]\n"
	}
}
FlowAnd := class extend FlowOrOrAnd
{	
	this := !() -> void
	{
		itFlowId = GetNewId()
	}
	PrintPre := virtual !(sfile f) -> void
	{
		Down.PrintPre(f)
		f << "br label %Start" << itFlowId << "\n"
		f << "Start" << itFlowId << ":\n"
		f << "br i1 " << Down.GetName() << ", label %OnBad" << itFlowId << " , label %OnGood" << itFlowId << "\n"
		f << "OnBad" << itFlowId << ":\n"
		Down.Right.PrintPre(f)
		f << "br label %BadFlow" << itFlowId << "\n"
		f << "BadFlow" << itFlowId << ":\n"
		//f << "%PreRes" << itFlowId << " = or i1 0, " << Down.Right.GetName() << "\n"
		f << "br label %OnGood" << itFlowId << "\n"
		f << "OnGood" << itFlowId << ":\n"
		f << "%Res" << itFlowId << " = phi i1 [0,%Start" << itFlowId << "] , [" <<Down.Right.GetName() << " , %BadFlow" << itFlowId << "]\n"
	}
}

