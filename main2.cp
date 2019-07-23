//printf := !(char^ a,...) -> int declare

c1 := !() -> void
{
	printf("start\n")
	on_exception printf("BAAAAAAAAD!\n")
	//throw new Exception(1,"YEEEEEEEEEES!")
}
c2 := !() -> void
{
	defer printf("YES\n")
	c1()
}


main := !() -> int
{
	x := int
	try
	{
		printf("hello\n")
		c2()
	}catch(IException^ strk) 
	{
		printf("exception <%s>\n",strk.Msg())
	}
	return 0
}

