//printf := !(char^ a,...) -> int declare


c := !( !(int)& -> int adder) -> void
{
	l := 0
	printf("heh %i\n" , adder(3))
	return void
}

main := !() -> int
{
	c(x => {
		defer printf("wow\n")
		return x + 10
	})
	return 0
}

