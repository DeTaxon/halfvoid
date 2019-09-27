
hoba := class
{
	c := int
	wut := !(!(int)&->void cb) -> void
	{
		printf("lambd\n")
		cb(1)
		cb(1)
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
	r.wut((x) ==>[x] {
		printf("heh %i\n",x)
	})
	return 0
}

