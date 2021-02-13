gRepo := vRepo

_hvEntryPoint := !(int argc, char^^ argv) -> int
{
	_TaskPtrReset()
	gRepo.Init(".")
	if $debug
	{
		gRepo.AddZipRoot("halfvoid")
		gRepo.AddZipRoot("halfvoid.exe")
	}
	LibDB.SetRepo(gRepo&)
	if $notask
	{
		return main(argc,argv)
	}else{
		tb := new TaskBox(4*1024*1024)
		preRet := int
		tb.Spawn(() ==> {
			preRet = main(argc,argv)
		})
		tb.Run()
		//delete tb
		return preRet
	}
}
