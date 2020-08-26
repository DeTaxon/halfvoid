
DownFunc := !() -> void
{
	throw new Exception("tast")
}

main := !(int argc, char^^ argv) -> int
{
	try 
	{
		switch argc
		{
			case 2
				return 13
			case 1
				DownFunc()
		}
	}catch()
	{
		printf("wut %s\n",e.Msg())
	}

	return 0
}
