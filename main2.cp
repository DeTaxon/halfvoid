
main := !() -> int
{
	k1 := (int adder) ==> {
		z := 0
		while true
		{
			printf("heh %i\n",z)
			z += adder
			yield void
		}
		return void
	}
	k1(13)
	k2 := k1.Capture()

	k1(1)
	k2(-1)
	k1(1)
	k2(-1)
	k1(1)
	k2(-1)
	k1(1)
	k2(-1)

	
	return 0
}

