
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
			WorkBag.Push(this&,State_GetUse)
		}
		if pri == State_GetUse
		{
			may := GetUse(this&)
			if may != null
			{
				ReplaceNode(this&,may)
			}else
			{
				ErrorLog.Push("unknown indent\n")
			}
		}
	}
}
