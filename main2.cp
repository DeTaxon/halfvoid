printf := !(char^ a,...) -> void declare

t := class
{
	io := int
	iop := virtual !() ->void
	{
		printf("nope\n")
	}
}

mistake := !() -> void
{
	c := t
	c.io = 0
	c.iop()
}

main := !() -> int
{
	printf("a\n")
	mistake()
	printf("b\n")
	//internalDeferAdd(() => {
	//	printf("hello world\n")
	//},false)
	//internalDeferApply(0,false)

	//{
	//	defer printf("hello world\n")
	//}
	return 0
}

