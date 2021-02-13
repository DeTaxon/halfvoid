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
	if $notask or true
	{
		return main(argc,argv)
	}else{
		tb := new TaskBox
		preRet := int
		tb.Spawn(() ==> {
			preRet = main(argc,argv)
		})
		tb.Run()
		//delete tb
		return preRet
	}
}
