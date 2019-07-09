#import "BoxFunc.cp"

WrappedFunc := class extend BoxFuncBody 
{
	this := !(Object^ itm) -> void
	{
		Down = itm
		itm.Up = this&
		Down.Left = null
		Down.Right = null
		MakeItBlock(Down)

		ABox.ItId = GetNewId()
		
		FuncName = "hiddenFunc"
		OutputName = "hiddenFunc" + ABox.ItId

		WorkBag.Push(this&,State_GetUse)
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
		if pri == State_GetUse
		{
			iter := Up

			while iter != null
			{
				if iter.GetValue() == "!()"
				{
					Parent = iter->{BoxFuncBody^}
					iter = null
				}else{
					iter = iter.Up
				}
			}

		}
	}
	GetValue := virtual !() -> string
	{
		return "{!()}"
	}
	PrintItCall := !(sfile f) -> void
	{
		nId := GetNewId()
		f << "%T" << nId << " = bitcast " << Parent.ABox.GetAsUse() << " to i8*\n"
		f << "call void(i8*)@" << OutputName <<"(i8* %T" << nId<<")\n"

	}

}
