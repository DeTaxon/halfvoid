WrappedFunc := class extend BoxFuncBody 
{
	this := !(Object^ itm) -> void
	{
		Line = itm.Line
		Down = itm
		itm.Up = this&
		Down.Left = null
		Down.Right = null
		MakeItBlock(Down)

		ABox.ItId = GetNewId()
		
		FuncName = "hiddenFunc"
		OutputName = StrCopy("hiddenFunc"sbt + ABox.ItId)

		//WorkBag.Push(this&,State_GetUse)
		MyFuncParamNames = new string[1]
		MyFuncParamNames[0] = "HiddenName"

		pars := Queue.{Type^}()
		
		vT := GTypeVoid

		pars.Push(vT.GetPoint())

		MyFuncType = GetFuncType(pars,null->{bool^},vT,false,false)

		ParseBlock()
	}
	DoTheWork := virtual !(int pri) -> void
	{
		//if pri == State_GetUse
		//{
		//	iter := Up

		//	while iter != null
		//	{
		//		if iter.GetValue() == "!()"
		//		{
		//			Parent = iter->{BoxFuncBody^}
		//			iter = null
		//		}else{
		//			iter = iter.Up
		//		}
		//	}

		//}
	}
	GetValue := virtual !() -> string
	{
		return "{!()}"
	}
	PrintItCall := !(sfile f) -> void
	{
		itB := AllocBox^()

		itrU := Up
		while itrU?.GetValue() != "!()" and itrU.GetValue() != "x=>x"
			itrU = itrU.Up

		assert(itrU != null)
		if itrU == null 
			return void
		if itrU.GetValue() == "x=>x"
			itB = itrU->{SLambda^}.ABox&
		if itrU.GetValue() == "!()"
			itB = itrU->{BoxFuncBody^}.ABox&
			
		nId := GetNewId()
		f << "%T" << nId << " = bitcast " << itB.GetAsUse() << " to i8*\n"
		f << "call void(i8*)@" << OutputName <<"(i8* %T" << nId<<")"
		if DebugMode
		{
			newId := CreateDebugCall(Up)
			if newId != -1
			{
				f << " , !dbg !" << newId 
			}
		}
		f << "\n"

	}

}
