gRepo := vRepo

_main := !(int argc, char^^ argv) -> int
{
	gRepo.Init(".")
	if $debug
	{
		gRepo.AddZipRoot("halfvoid")
		gRepo.AddZipRoot("halfvoid.exe")
	}
	return main(argc,argv)
}
