Screamer := class extend TGCObject
{
	Destroy := virtual !() -> void
	{
		printf("TOAST\n")
	}
}


main := !(int argc, char^^ argv) -> int
{
	c := new Screamer^[13]
	//if c == null
	//	printf("nope\n")
	printf("t %s\n",c->Type->Name)
	return 0
}
