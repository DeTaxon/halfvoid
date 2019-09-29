
hoba := class
{
	c := int
	wut := !(!(int)&->void cb) -> !(int)&->void
	{
		printf("lambd\n")
		cb(1)
		cb(1)
		return cb.Capture()
	}
	wut := !(!(int)^->void cb) -> void
	{
		printf("func\n")
		cb(1)
	}

}
main := !() -> int
{
	r := hoba
	x := 13
	z := r.wut((y) ==> [x] {
		printf("heh %i\n",x)
		x += 3
	})
	z(3)
	printf("c %i\n",x)
	return 0
}

