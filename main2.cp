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

func2 := !() -> void
{
}
main := !(int argc, char^^ argv) -> int
{
	func2()	
	//printf("hello\n")
	//gRepo.PreferVirtual(true)
	//gRepo.AddZipRoot("./Dlls.zip")
	//fil := gRepo.GetFile("./ILU.dll")
	//printf("fil %p\n",fil)
	//printf("is virtual %i\n", fil?.IsVirtual() ?? false)
	//c := new Screamer^[10]
	//printf("ptr %p %s\n",c,c->Type->Name)
	//printf("hoh %p %p %p\n",c[0],c[1],c[2])
	//internalTGCArrayGetElement(c,0) = new Screamer
	//c[0] = null
	//printf("hoh %p %p %p\n",c[0],c[1],c[2])
	//z(c)
	return 0
}
