
hoba := class
{
	c := int
	wut := !(!()&->void cb) -> void
	{
		printf("lambd\n")
		cb()
	}
	wut := !(!()^->void cb) -> void
	{
		printf("func\n")
		cb()
	}

}
main := !() -> int
{
	r := hoba
	r.wut(() => {
		pr := 13
		defer printf("heh %i\n",pr)
	})
	return 0
}

