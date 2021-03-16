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
	}
	GetValue := virtual !() -> string
	{
		return "{!()}"
	}
	PrintItCall := !(TIOStream f) -> void
	{
		itrU := Up
		while itrU != null and itrU.GetABox() == null
			itrU = itrU.Up

		assert(itrU != null)
		if itrU == null 
			return void
		itB := itrU.GetABox()
			
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
