ObjIndent := class extend Object
{
	MyStr := char^
	isLambdaNum := bool
	"this" := !(char^ str) -> void
	{
		MyStr = str
		if str != null and str[0] == '_'
		{
			isLambdaNum = true
			i := 1
			while str[i] != 0
			{
				if not (str[i] in '0'..'9')
				{
					isLambdaNum = false
					break
				}
				i += 1
			}
			if i == 1
				isLambdaNum = false
		}
	}
	GetValue := virtual !() -> char^
	{
		return "~ind"
	}
	Print := virtual !(int s) -> void
	{
		for s printf("->")
		printf("indent %s\n",MyStr)
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			WorkBag.Push(this&,State_PreGetUse)
		}
		if pri == State_PreGetUse
		{
			WorkBag.Push(this&,State_GetUse)
			ItFuncs := Queue.{ObjParam^}()
			CollectParamsAllByName(MyStr,this&,ItFuncs)

			iter0 := ItFuncs.Start
			while iter0 != null
			{
				WorkBag.Push(iter0.Data,State_CheckTypes)
				iter0 = iter0.Next
			}
		}
		if pri == State_GetUse
		{
			may := GetUse(this&)

			if may != null
			{
				ReplaceNode(this&,may)
			}else
			{
				ItFuncs := Queue.{BoxFunc^}()
				ItTempls := Queue.{BoxTemplate^}()
				Searched := QueueSet.{int}()
				CollectFuncsByName(MyStr,this&,ItFuncs,ItTempls,false,false,Searched,false)
				if ItFuncs.Size() == 0
				{
					Typ := ParseType(this&)
					if Typ != null
					{
						ReplaceNode(this&,new ObjType(Typ) )
					}else{
						ignore := false
						if Left != null 
						{
							if Left.GetValue() == "." ignore = true
						}
					}
				}
			}
		}
	}
	Clone := virtual !() -> Object^
	{
		PreRet := new ObjIndent(MyStr)
		PreRet.isLambdaNum = isLambdaNum
		PreRet.Line = Line
		return PreRet
	}
}
ObjSuffix := class extend ObjConst
{
	MyStr := char^
	"this" := !(char^ str) -> void
	{
		MyStr = StrCopy(str)
	}
	GetValue := virtual !() -> char^
	{
		return "~suffix"
	}
	Print := virtual !(int s) -> void
	{
		for s printf("->")
		printf("suffix %s\n",MyStr)
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			WorkBag.Push(this&,State_PreGetUse)
		}
		if pri == State_PreGetUse
		{
			ItFuncs := Queue.{ObjParam^}()
			CollectParamsAllByName(MyStr,this&,ItFuncs)

			iter0 := ItFuncs.Start
			while iter0 != null
			{
				WorkBag.Push(iter0.Data,State_CheckTypes)
				iter0 = iter0.Next
			}
		}
	}
	Clone := virtual !() -> Object^
	{
		PreRet := new ObjSuffix(MyStr)
		PreRet.Line = Line
		return PreRet
	}
}
