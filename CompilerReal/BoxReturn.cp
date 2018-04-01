BoxReturn := class extend Object
{
	this := !(Object^ toUse) -> void
	{
		PopOutNode(toUse.Down)
		Down = toUse.Down
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			WorkBag.Push(this&,State_Syntax)
			Down.SetUp(this&)
		}
		if pri == State_Syntax
		{
			WorkBag.Push(Down,State_Start)
			WorkBag.Push(this&,State_GetUse)
		}
		if pri == State_GetUse
		{
			Down.SetUp(this&)

			iterF := Up

			lazy := iterF != null
			if lazy lazy = iterF.GetValue() != "!()"
			while lazy
			{
				iterF = iterF.Up
				lazy = iterF != null
				if lazy lazy = iterF.GetValue() != "!()"
			}
			if iterF != null
			{
				asNeed := iterF->{BoxFunc^}
				if Down.GetType() != null
				{
					PreType := asNeed.MyFuncType.RetType
					if PreType == null
					{
						asNeed.SetReturnType(Down.GetType())
					}else
					{
						if PreType != Down.GetType()
						{
							if TypeCmp(Down.GetType(),PreType) != 255
							{
								Down = BoxExc(Down,PreType,asNeed.MyFuncType.RetRef)
								Down.Up = this&
							}else{
								ErrorLog.Push("Can not return value")
							}
						}
					}
				}else{
					ErrorLog.Push("recursion detected\n")
				}
			}else{
				ErrorLog.Push("impossible state\n")
			}
		}
	}
	PrintInBlock := virtual !(sfile f) -> void
	{
		Down.PrintPre(f)
		f << "ret "
		Down.PrintUse(f)
		f << "\n"
	}
	GetValue := virtual !() -> string
	{
		return "~Return()"
	}
}
