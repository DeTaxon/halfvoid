ObjData := class extend Object
{
	this := !() -> void
	{
	}
	GetValue := virtual !() -> string
	{
		return "~d"
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_PreGetUse
		{
			iter := Down
			while iter != null
			{
				if iter.GetValue() == "()" WorkBag.Push(iter,State_Syntax)
				WorkBag.Push(iter,State_PreGetUse)
				iter = iter.Right
			}
			WorkBag.Push(this&,State_GetUse)
		}
		if pri == State_GetUse
		{
			lowTest := GetUse(this&)
			if lowTest == null ErrorLog.Push("can not parse data\n")
			else
			{
				printf("heillo\n")
			}
		}

	}
}
