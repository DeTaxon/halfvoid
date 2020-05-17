
main := !() -> int
{
	oo := 3
	x := 3
	y := 5
	z := ref x
	r := () ==> [oo,x,y&] 
	{
		k := x + y
		printf("wut %i\n",k)
		x += 100
		y += 100
	}
	r()
	printf("after %i %i\n",x,y)
	return 0
}
