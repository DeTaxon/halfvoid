#import "Tree.cp"

WayControl := class extend Object
{
	itItm := string
	toName := string
	itType := int
	itSize := int
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
	PrintInBlock := virtual !(sfile f) -> void
	{
		f << "br label %" << toName << "\n"
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

			while iter != null
			{
				lazy := false
				if iter.GetValue() == "~~for()" lazy = true
				if iter.GetValue() == "~while()" lazy = true

				if lazy
				{
					iter = null
				}else{
					if iter.GetValue() == "{d}" itSize += 1
					iter = iter.Up
				}
			}
			toName = Up.GetOutPath(this&,itType,itSize)
		}
	}
}
