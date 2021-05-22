ControlFlowBox := class extend Object
{
	GetValue := virtual !() -> string
	{
		return "?or??"
	}
	PrintInBlock := virtual !(TIOStream f) -> void { PrintPre(f)}

	GetBadLabel := virtual !(Object^ prevNode) -> BoxLabel^
	{
		assert(false)
		return null
	}
}

IsQBox := !(Object^ toCmp) -> bool
{
	if toCmp is QuestionBox return true
	if toCmp is QuestionBoxRef return true
	return false
}

QJumpLand := class extend Object
{
	endLabel := BoxLabel^
	this := !() -> void
	{
		endLabel = new BoxLabelAnon()
	}
	GetEndLabel := virtual !() -> BoxLabel^
	{
		return endLabel
	}
	PrintInBlock := virtual !(TIOStream f) -> void { 
		Down.PrintInBlock(f)
		f << "br label %" << endLabel.GetLabel() << "\n"
		endLabel.PrintLabel(f)
	}
	PrintPre := virtual !(TIOStream f) -> void { 
		Down.PrintPre(f) 
	}
	DoTheWork := virtual !(int pri) -> void
	{
		Down.DoTheWork(pri) //not ok but it's actualy ok
	}
}

QAtleastBox := class extend ControlFlowBox 
{
	onFalse := BoxLabel^
	endLabel := BoxLabel^
	itId := int
	this := !() -> void
	{
		onFalse = new BoxLabelAnon()
		endLabel = new BoxLabelAnon()
		itId = GetNewId()
	}
	GetEndLabel := virtual !() -> BoxLabel^
	{
		return endLabel
	}
	IsRef := virtual !() -> bool
	{
		return Down.IsRef() and Down.Right.IsRef()
	}
	GetType := virtual !() -> Type^
	{
		return Down.GetType()
	}
	GetBadLabel := virtual !(Object^ prevNode) -> BoxLabel^
	{
		if Down == prevNode
			return onFalse
		return null
	}
	PrintPointPre := virtual !(TIOStream f) -> void
	{
		f << "br label %Start" << itId << "\n"
		f << "Start" << itId << ":\n"
		Down.PrintPointPre(f)
		f << "br label %StartEnd" << itId << "\n"
		f << "StartEnd" << itId << ":\n"
		f << "br label %" << endLabel.GetLabel() << "\n"

		onFalse.PrintLabel(f)
		Down.Right.PrintPointPre(f)
		f << "br label %LastFalse" << itId << "\n"
		f << "LastFalse" << itId << ":\n"
		f << "br label %" << endLabel.GetLabel() << "\n"

		endLabel.PrintLabel(f)
		f << "%Value" << itId << " = phi "
		Down.GetType().GetPoint().PrintType(f)
		f << " ["<< Down.GetPointName() <<",%StartEnd"<<itId<<"]" 

		f <<", ["<< Down.Right.GetPointName()<<",%LastFalse"<< itId <<"]\n" 
	}
	PrintPointUse := virtual !(TIOStream f) -> void
	{
		Down.GetType().PrintType(f)
		f << "* %Value" << itId
	}
	GetName := virtual !() -> char^
	{
		return "%Value"sbt + itId
	}
	PrintPre := virtual !(TIOStream f) -> void
	{
		f << "br label %Start" << itId << "\n"
		f << "Start" << itId << ":\n"
		Down.PrintPre(f)
		f << "br label %StartEnd" << itId << "\n"
		f << "StartEnd" << itId << ":\n"
		f << "br label %" << endLabel.GetLabel() << "\n"

		onFalse.PrintLabel(f)
		Down.Right.PrintPre(f)
		f << "br label %LastFalse" << itId << "\n"
		f << "LastFalse" << itId << ":\n"
		f << "br label %" << endLabel.GetLabel() << "\n"

		endLabel.PrintLabel(f)
		f << "%Value" << itId << " = phi "
		Down.GetType().PrintType(f)
		f << " ["<< Down.GetName() <<",%StartEnd"<<itId<<"]"
		f <<", ["<< Down.Right.GetName()<<",%LastFalse"<< itId <<"]\n" 
	}
	PrintUse := virtual !(TIOStream f) -> void
	{
		Down.GetType().PrintType(f)
		f << " %Value" << itId
	}
	GetName := virtual !() -> char^
	{
		return "%Value"sbt + itId
	}
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
			tp1 := Down.GetType()
			tp2 := Down.Right.GetType()
			if tp1 == null
				EmitError("Left type is null")
			if tp2 == null
				EmitError("Right type is null")
			newType := TypeFight(tp1,tp2)
			if newType == null{
				Print(0)
				EmitError("can not unite types ")
			}
			if tp1 != newType BoxExc(Down,newType,false)
			if tp2 != newType BoxExc(Down.Right,newType,false)
		}
	}
	GetDebugValue := virtual !() -> char^
	{
		return "??"
	}
}
QuestionBoxRef2 := class extend QuestionBox2
{
	PrintPre := virtual !(TIOStream f) -> void
	{
		PrintPointPre(f)
		f << "%QValue"<< itId <<" = load " << Down.GetType().GetName() << " , "
		Down.PrintPointUse(f) 
		f << "\n"
	}
	PrintUse := virtual !(TIOStream f) -> void { 
		f << Down.GetType().GetName() << " %QValue" << itId
	}
	GetName := virtual !() -> char^ {
		return "%QValue"sbt + itId
	}
}
QuestionBox2 := class extend Object
{	
	itId := int
	onBadLabel  := BoxLabel^
	this := !() -> void
	{
		itId = GetNewId()
	}
	IsRef := virtual !() -> bool
	{
		return Down.IsRef()
	}
	PrintPointPre := virtual !(TIOStream f) -> void
	{
		dwnType := Down.GetType()

		Down.PrintPointPre(f)
		f << "br label %Start" << itId << "\n"
		f << "Start" << itId << ":\n"
		f << "%CmpRes" << itId << " = icmp ne " 
		Down.PrintPointUse(f)
		f << " , null\n"
		f << "br i1 %CmpRes" << itId << ", label %OnGood" << itId <<", label %"<< onBadLabel.GetLabel() << "\n"
		f << "OnGood" << itId << ":\n"
	}
	PrintInBlock := virtual !(TIOStream f) -> void { PrintPre(f)}
	PrintPre := virtual !(TIOStream f) -> void
	{
		dwnType := Down.GetType()

		if dwnType is TypePoint or dwnType is TypeFatArr
		{
			Down.PrintPre(f)
			f << "br label %Start" << itId << "\n"
			f << "Start" << itId << ":\n"
			f << "%CmpRes" << itId << " = icmp ne " 
			Down.PrintUse(f)
			f << " , null\n"
			f << "br i1 %CmpRes" << itId << ", label %OnGood" << itId <<", label %"<< onBadLabel.GetLabel() << "\n"
			f << "OnGood" << itId << ":\n"
		}else{
			assert(false)
		}
	}
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
			if Down is NaturalCall //TODO: NaturalCallRef
			{
				asCall := Down->{NaturalCall^}
				cFunc := asCall.ToCall
				
				if cFunc.FuncName == "[]" and cFunc.IsMethod and cFunc.MethodType? is TypeClass
				{
					funcHolder := GetBoxClassFuncsHolder(cFunc)

					if funcHolder != null
					{
						inMethods := funcHolder.methods.TryFind("[]?")
						if inMethods != null
						{
							for func : inMethods^
							{
								if(CmpFuncInputOnly(func.MyFuncType,cFunc.MyFuncType)){
									asCall.ToCall = func
									asCall.FType = func.MyFuncType //TODO: stupid, unite with line up
									func.ParseBlock()
									break
								}
							}
						}
					}
				}
			}

			dwnType := Down.GetType()
			if dwnType == null
			{
				Print(0)
			}
			if dwnType is TypeFatArr or dwnType is TypePoint
			{	
				//nothing
			}else{
				if not Down.IsRef()
				{
					EmitError("question type is not a pointer, its "sbt + dwnType.GetType() + " \n")
				}else{
					cc := this&
					cc->SetType(QuestionBoxRef2)//TODO: this->SetType 
				}
			}
			
			visitedWork << pri
		}
	}
	GetType := virtual !() -> Type^ {
		return Down.GetType()
	}
	PrintPointUse := virtual !(TIOStream f) -> void { 
		Down.PrintPointUse(f)
	}
	GetPointName := virtual !() -> char^ {
		return Down.GetPointName()
	}
	PrintUse := virtual !(TIOStream f) -> void { 
		Down.PrintUse(f)
	}
	GetName := virtual !() -> char^ {
		return Down.GetName()
	}
	GetValue := virtual !() -> char^
	{
		return "d?"
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
	PrintUse := virtual !(TIOStream f) -> void { f << "i1 %Res" << itFlowId }
	GetName := virtual !() -> char^ {
		return "%Res"sbt + itFlowId 
	}
	
}

FlowOr := class extend FlowOrOrAnd
{	
	this := !() -> void
	{
		itFlowId = GetNewId()
		onBadLabel = new BoxLabelAnon()
	}
	PrintPre := virtual !(TIOStream f) -> void
	{
		Down.PrintPre(f)
		f << "br label %Start" << itFlowId << "\n"
		f << "Start" << itFlowId << ":\n"
		f << "br i1 " << Down.GetName() << ", label %OnGood" << itFlowId << " , label %" << onBadLabel.GetLabel() << "\n"
		onBadLabel.PrintLabel(f)
		Down.Right.PrintPre(f)
		f << "br label %BadFlow" << itFlowId << "\n"
		f << "BadFlow" << itFlowId << ":\n"
		//f << "%PreRes" << itFlowId << " = or i1 0, " << Down.Right.GetName() << "\n"
		f << "br label %OnGood" << itFlowId << "\n"
		f << "OnGood" << itFlowId << ":\n"
		f << "%Res" << itFlowId << " = phi i1 [1,%Start" << itFlowId << "] , [" <<Down.Right.GetName() << " , %BadFlow" << itFlowId << "]\n"
	}
	onBadLabel  := BoxLabel^
	GetBadLabel := virtual !(Object^ prevNode) -> BoxLabel^
	{
		if Down == prevNode
			return onBadLabel
		return null
	}
}
FlowAnd := class extend FlowOrOrAnd
{	
	this := !() -> void
	{
		itFlowId = GetNewId()
	}
	PrintPre := virtual !(TIOStream f) -> void
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

		if onBadLabel == null
		{
			f << "%Res" << itFlowId << " = phi i1 [0,%Start" << itFlowId << "] , [" <<Down.Right.GetName() << " , %BadFlow" << itFlowId << "]\n"
		}else{
			f << "%PreRes" << itFlowId << " = phi i1 [0,%Start" << itFlowId << "] , [" <<Down.Right.GetName() << " , %BadFlow" << itFlowId << "]\n"
			f << "br label %AlmostLast" << itFlowId << "\n"
			f << "AlmostLast" << itFlowId << ":\n"
			f << "br label %NewLast" << itFlowId << "\n"
			onBadLabel.PrintLabel(f);
			f << "br label %NewLast" << itFlowId << "\n"
			f << "NewLast" << itFlowId << ":\n"
			f << "%Res" << itFlowId << " = phi i1 [0,%" << onBadLabel.GetLabel() << "] , [%PreRes" <<itFlowId <<" , %AlmostLast" << itFlowId << "]\n"
		}
	}
	onBadLabel  := BoxLabel^
	GetBadLabel := virtual !(Object^ prevNode) -> BoxLabel^
	{
		if Down == prevNode
		{
			if onBadLabel == null
				onBadLabel = new BoxLabelAnon()
			return onBadLabel
		}
		return null
	}
}

