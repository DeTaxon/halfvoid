gRepo := vRepo

_main := !(int argc, char^^ argv) -> int
{
	_TaskPtrReset()
	gRepo.Init(".")
	if $debug
	{
		gRepo.AddZipRoot("halfvoid")
		gRepo.AddZipRoot("halfvoid.exe")
	}
	LibDB.SetRepo(gRepo)
	return main(argc,argv)
}
