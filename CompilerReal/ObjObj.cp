
ObjObj := class extend Object
{
	MyStr := char^
	"this" := !(char^ st) -> void
	{
		Clean()
		MyStr = st
	}
	GetValue := virtual !() -> char^
	{
		return MyStr
	}
	Print := virtual !(int s) -> void
	{
		for s printf("->")
		printf("object %s\n",MyStr)

		iter := Down
		while iter != null {
			iter.Print(s+1)
			iter = iter.Right
		}
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			Found := false
			if MyStr == "if()"
			{
				It := this&->{Object^}
				NewNode := new BoxIf(It)
				ReplaceNode(this&,NewNode)
				WorkBag.Push(NewNode,State_Start)
				Found = true
			}
			if MyStr == "while()"
			{
				It := this&->{Object^}
				NewNode := new BoxWhile(It)
				ReplaceNode(this&,NewNode)
				WorkBag.Push(NewNode,State_Start)
				Found = true
			}
			if MyStr == "return()"
			{
				It := this&->{Object^}
				NewNode := new BoxReturn(It)
				ReplaceNode(this&,NewNode)
				WorkBag.Push(NewNode,State_Start)
				Found = true
			}
			if MyStr == "for()"
			{
				It := this&->{Object^}
				NewNode := GetBoxFor(It)->{Object^}
				ReplaceNode(this&,NewNode)
				WorkBag.Push(NewNode,State_Start)
				Found = true
			}
			if not Found
			{
				//WorkBag.Push(this&,State_PreGetUse)
			}
		}
		if pri == State_PreGetUse
		{
			iterQ := Down
			while iterQ != null
			{
				WorkBag.Push(iterQ,State_Start)
				iterQ = iterQ.Right
			}
			WorkBag.Push(this&,State_GetUse)
		}
		if pri == State_GetUse
		{
			Maybe := GetUse(this&)

			if Maybe != null
			{
				ReplaceNode(this&,Maybe)
				printf("nope\n")
			}
		}
	}
}
