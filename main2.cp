
main := !() -> int
{
	x := 13
	k := () ==> [x&]{
		printf("hoh %i\n",x)
		x += 13
	}
	k()
	z := k.Capture()
	z()
	printf("hop %i\n",x)
	z.Destroy()
	return 0
}
