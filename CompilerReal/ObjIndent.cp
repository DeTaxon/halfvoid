
ObjIndent := class extend ObjConst
{
	MyStr := char^
	"this" := !(char^ str) -> void
	{
		Clean()
		MyStr = str
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
				CollectFuncsByName(MyStr,this&,ItFuncs,ItTempls,false)
				if ItFuncs.Size() == 0 
					ErrorLog.Push("unknown indent "+ MyStr + "\n")
			}
		}
	}
	Clone := virtual !() -> Object^
	{
		PreRet := new ObjIndent(MyStr)
		PreRet.Line = Line
		return PreRet
	}
}
ObjSuffix := class extend ObjConst
{
	MyStr := char^
	"this" := !(char^ str) -> void
	{
		Clean()
		MyStr = str.Copy()
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
