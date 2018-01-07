ObjParam := class extend Object
{
	MyStr := string
	
	this := !(string ss) -> void
	{
		MyStr
	}
	GetValue := !() -> string
	{
		return "i:=1"
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
}
