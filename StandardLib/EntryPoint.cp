gRepo := vRepo

_hvEntryPoint := !(int argc, char^^ argv,char^^ envs) -> int
{
	_TaskPtrReset()
	_taskInitMem(_TaskPtrGet())
	gRepo.Init(".")
	if $debug
	{
		gRepo.AddZipRoot("halfvoid")
		gRepo.AddZipRoot("halfvoid.exe")
	}

	argv2 := argv

	if $win32
	{
		SetConsoleOutputCP(CP_UTF8)
		SetConsoleCP(CP_UTF8)
		
		cmd := GetCommandLineW()
		argv2W := CommandLineToArgvW(cmd,argc&)
		for i : argc
		{
			argv2[i] = StrCopy(Utf16ToUtf8(argv2W[i]))
		}
		LocalFree(argv2W)
	}

	LibDB.SetRepo(gRepo&)

	preRet := 0
	if $notask
	{
		preRet = main(argc,argv2)
	}else{
		tb := new TaskBox(4*1024*1024)
		tb.Spawn(() ==> {
			preRet = main(argc,argv2)
		})
		tb.Run()
		//delete tb
	}
	if $win32
	{
		for i : argc
		{
			delete argv2[i]
		}
	}
	return preRet
}