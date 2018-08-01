WayControl := class extend Object
{
	itItm := string
	this := !(string itm) -> void
	{
		itItm = itm
	}
	Clone := virtual !() -> Object^
	{
		return new WayControl(itItm)
	}
}
