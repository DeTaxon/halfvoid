

startTimer := double
main := !(int argc, char^^ argv) -> int
{
	startTimer = TGetSteadyTime()
	folders := List.{Tuple.{char^,bool}}()
	cmd := ""

	i := 1
	while i < argc
	{
		switch(argv[i])
		{
			case "-f"
				i += 1
				folders.Emplace(argv[i],false)
			case "-cmd"
				i += 1
				cmd = argv[i]
		}
		i += 1
	}

	
	for fldr : folders
	{
		TMonitor(fldr.0,M_CHANGED,fldr.1,(x,y) ==> {
			nowTime := TGetSteadyTime()
			if nowTime - startTimer > 1.0
			{
				system(cmd)
			}
			startTimer = nowTime
		})
	}
	yield 0
}
