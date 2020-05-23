
main := !() -> int
{
	a := 0
	b := a&
	r := () ==>  
	{
		r2 := () ==> {
			x := 0
			while true
			{
				b^ += 1
				printf("heh %i\n",b^)
				yield void
			}
		}
		while true
		{
			r2()
			yield void
		}
	}
	r()
	r()
	r()
	r()
	return 0
}
