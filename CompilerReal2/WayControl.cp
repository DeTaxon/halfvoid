WayControl := class extend Object
{
	itItm := string
	itType := int
	itSize := int
	itLabel := BoxLabel^
	this := !(string itm) -> void
	{
		itType = PATH_CONTINUE
		if itm == "break" itType = PATH_BREAK
		itItm = itm
	}
	Clone := virtual !() -> Object^
	{
		return new WayControl(itItm)
	}
	PrintInBlock := virtual !(TIOStream f) -> void
	{
		f << "br label %" << itLabel.GetLabel() << "\n"
	}
	GetValue := virtual !() -> string
	{
		if itItm == "break" return "~break"
		return "~continue"
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			WorkBag.Push(this&,State_DestructGet)
		}
		if pri == State_DestructGet
		{
			itSize = 0
			iter := Up
			prevNode := iter

			while iter != null
			{
				if iter.GetValue() in !["~~for()","~while()","~switch()"]
				{
					asBl := prevNode->{BoxBlock^}
					if asBl.callDeferStuf
					{
						iter.ApplyDeferUse(1)
					}
					iter = null
				}else{
					if iter.GetValue() == "{d}" itSize += 1
					prevNode = iter
					iter = iter.Up
				}
			}
			itLabel = Up.GetOutPath(this&,itType,itSize)
		}
	}
}
