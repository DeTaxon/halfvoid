Screamer := class extend TGCObject
{
	Destroy := virtual !() -> void
	{
		printf("TOAST\n")
	}
}

z := !(Screamer^[] h) -> void
{
	//h[1] = new Screamer
}


main := !(int argc, char^^ argv) -> int
{
	c := new Screamer^[10]
	printf("ptr %p %s\n",c,c->Type->Name)
	printf("hoh %p %p %p\n",c[0],c[1],c[2])
	internalTGCArrayGetElement(c,0) = new Screamer
	c[0] = null
	printf("hoh %p %p %p\n",c[0],c[1],c[2])
	z(c)
	return 0
}
