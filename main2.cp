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
	c[0] = new Screamer
	return 0
}
