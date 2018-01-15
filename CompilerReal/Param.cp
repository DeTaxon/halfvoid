ObjParam := class extend Object
{
	MyStr := string
	AskedGetUse := bool
	ObjType := Type^
	
	this := !(string ss) -> void
	{
		MyStr = ss
	}
	GetValue := virtual !() -> string
	{
		return "i:=1"
	}
	GetType := virtual !() -> Type^
	{
		return ObjType
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
	GetName := virtual !() -> string
	{
		return MyStr
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			WorkBag.Push(this&,State_CheckTypes)

			iter := Down

			while iter != null
			{
				if iter.GetValue() == "()"
					WorkBag.Push(iter,State_Syntax)
				iter = iter.Right
			}
		}

		if pri == State_CheckTypes
		{
			MaybeType := ParseType(Down)

			if MaybeType != null
			{
				allcId := GetAlloc(this&,MaybeType)
				ObjType = MaybeType
				if allcId == -1
				{
					//TODO: Global?
				}else
				{
					Down = new LocalParam(MaybeType,allcId)
				}
			}else
			{
				SomeObj := GetUse(Down)
				
				if SomeObj != null
				{
					Down = SomeObj
					SomeObj.SetUp(this&)
					ObjType = SomeObj.GetType()
				}else
				{
					printf("compiler error\n")
				}
			}
		}
		if pri == State_PreGetUse
		{
			WorkBag.Push(Down,State_Start)
			WorkBag.Push(this&,State_GetUse)
		}
		if pri == State_GetUse
		{
			AskedGetUse = true
			WorkBag.Push(Down,State_Start)
		}
	}
}
