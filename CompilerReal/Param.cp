ObjParam := class extend Object
{
	MyStr := string
	
	this := !(string ss) -> void
	{
		MyStr
	}
	GetValue := virtual !() -> string
	{
		return "i:=1"
	}
	Print := virtual !(int lvl) -> void
	{
		for lvl printf("->")
		printf("param i:=1\n")
		iter := Down
		while iter != null
		{
			iter.Print(lvl+1)
			iter = iter.Right
		}
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			WorkBag.Push(this&,State_CheckTypes)
		}

		if pri == State_CheckTypes
		{
			MaybeType := ParseType(Down)

			if MaybeType != null
			{
			}else
			{
				SomeObj := GetUse(Down)
				
				if SomeObj != null
				{
					Down = SomeObj
					SomeObj.SetUp(this&)
				}else
				{
					printf("compiler error\n")
				}
			}
		}
	}
}
