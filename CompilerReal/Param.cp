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
	//PrintGlobal := !(sfile f) -> void
	//{
	//	if not IsFunc or AskedGetUse
	//	{
	//		Down.PrintGlobal(f)
	//	}
	//}

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
			val := Object^
			val = null

			if MaybeType == null
			{
				val = TryCompute(Down)
				if val != null
				{
					MaybeType = val.GetType()
				}
			}

			if MaybeType != null
			{
				allcId := GetAlloc(this&,MaybeType)
				ObjType = MaybeType
				if allcId == -1
				{
					//TODO: Global?
					Down = new GlobalParam(MaybeType,val)
					Down.SetUp(this&)
				}else
				{
					if allcId == -2
					{
						asCl := GetUpClass(this&)
						if asCl != null
						{
							Down = new FieldParam(MyStr,MaybeType,asCl)
							Down.SetUp(this&)
						}else{
							ErrorLog.Push("Compiler bag\n")
						}
					}else{
						Down = new LocalParam(MaybeType,allcId)
						Down.SetUp(this&)
					}
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
					//WorkBag.Push(this&,State_GetUse)
				}else
				{
					asClass := ParseClass(Down)
					if asClass != null
					{
						Down = asClass
						asClass.Up = this&
					}else{
						WorkBag.Push(this&,State_PreGetUse)
					}
				}
			}
		}
		if pri == State_PreGetUse
		{
			if Down.GetValue() != "i:=2"
			{
				WorkBag.Push(Down,State_Start)
				WorkBag.Push(this&,State_GetUse)
			}
		}
		if pri == State_GetUse
		{
			if IsFunc 
			{
				AskedGetUse = true
			}else{
				if Down.GetValue() != "i:=2"
				{
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
}
