
tr := class
{
	x := int
	y := float
}


tst := !(tr tst,!(tr)&->bool unp) -> void
{
	if unp(tst)
		printf("value\n")
}

main := !() -> int
{
	o := tr
	o.x = 3
	tst(o,_1.x > 0)
	return 0
}
