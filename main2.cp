
bag := class
{
	odd := fake
	{
		"in" := !(int x) -> bool
		{
			return (x and_b 1)
		}
	}
	"in" := !(int x) -> bool
	{
		return false
	}
}


main := !(int argc, char^^ argv) -> int
{
	tst := bag
	printf("tst %i\n",10[^] in tst.odd)
	return 0
}
