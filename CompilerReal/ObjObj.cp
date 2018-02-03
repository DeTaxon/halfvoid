
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
			if MyStr == "if()"
			{
				It := this&->{Object^}
				NewNode := new BoxIf(It)
				ReplaceNode(this&,NewNode)
				WorkBag.Push(NewNode,State_Start)
			}
			if MyStr == "while()"
			{
				It := this&->{Object^}
				NewNode := new BoxWhile(It)
				ReplaceNode(this&,NewNode)
				WorkBag.Push(NewNode,State_Start)
			}
			if MyStr == "return()"
			{
				It := this&->{Object^}
				NewNode := new BoxReturn(It)
				ReplaceNode(this&,NewNode)
				WorkBag.Push(NewNode,State_Start)
			}
		}
	}
}
