ObjObj := class extend Object
{
	MyStr := char^
	"this" := !(char^ st) -> void
	{
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
			It := this&->{Object^}

			switch MyStr
			{
			case  "if()"
				NewNode := new BoxIf(It)
				ReplaceNode(this&,NewNode)
				WorkBag.Push(NewNode,State_Start)
			case "while()"
				NewNode := new BoxWhile(It)
				ReplaceNode(this&,NewNode)
				WorkBag.Push(NewNode,State_Start)
			case "return()"
				NewNode := new BoxReturn(It)
				ReplaceNode(this&,NewNode)
				WorkBag.Push(NewNode,State_Start)
			case "for()"
				NewNode := GetBoxFor(It)->{Object^}
				ReplaceNode(this&,NewNode)
				WorkBag.Push(NewNode,State_Start)
			case "Defer()"
				preSet := It.Down.Right
				osEx := It.Down.GetValue() == "on_exception"
				PopOutNode(preSet)
				NewNode := new ObjDefer(preSet,osEx)
				ReplaceNode(this&,NewNode)
			case "switch()"
				NewNode := new BoxSwitch(It)
				ReplaceNode(this&,NewNode)
				WorkBag.Push(NewNode,State_Start)
			case "case()"
				NewNode := new BoxCase(It)
				ReplaceNode(this&,NewNode)
				WorkBag.Push(NewNode,State_Start)
			case "i.{}:=0"
				newNode := new BoxTemplateTypedef(It) 
				ReplaceNode(this&,newNode)
				newNode.ApplyDown(this&)
			case void
				//WorkBag.Push(this&,State_PreGetUse)
			}
		}
		if pri == State_PreGetUse
		{
			WorkBag.Push(Down[^],State_Start)
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
