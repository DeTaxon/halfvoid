ObjParam := class extend Object
{
	MyStr := string
	AskedGetUse := bool
	ObjType := Type^
	IsSetValue := bool
	IsFunc := bool
	IsStrName := bool
	
	this := !(string ss, bool IsstName) -> void
	{
		MyStr = ss.Copy()
		IsStrName = IsstName
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
	PrintInBlock := virtual !(sfile f) -> void
	{
		if IsSetValue 
		{
			asLoc := Down->{LocalParam^}
			Down.Right.PrintPre(f)
			f << "store "
			Down.Right.PrintUse(f)
			f << " , "
			asLoc.PrintPointUse(f,0)
			f << "\n"
		}
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
					Down = new GlobalParam(MaybeType)
				}else
				{
					Down = new LocalParam(MaybeType,allcId)
				}
			}else
			{
				SomeObj := ParseFuncDataR(Down)
				
				if SomeObj != null 
				{
					IsFunc = true
					Down = SomeObj
					SomeObj.SetUp(this&)
					ObjType = SomeObj.GetType()
					WorkBag.Push(this&,State_GetUse)
				}else
				{
					WorkBag.Push(this&,State_PreGetUse)
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
			if IsFunc 
			{
				AskedGetUse = true
				WorkBag.Push(Down,State_Start)
			}else{
				ObjType = Down.GetType()
				if ObjType != null
				{
					Temp := Down
					allcId := GetAlloc(this&,ObjType)
					Down = new LocalParam(ObjType,allcId)
					Down.Right = Temp
					Temp.Left = Down
					Down.SetUp(this&)
					IsSetValue = true
				}
			}
		}
	}
}
