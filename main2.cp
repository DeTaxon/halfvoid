
bag := class
{
	odd := fake
	{
		"in" := !(int x) -> bool
		{
			return (x and_b 1)
		}
	}
	even := fake
	{
		"in" := !(int x) -> bool
		{
			return not (x in this.odd)
		}
	}
}


main := !(int argc, char^^ argv) -> int
{
	tst := bag
	printf("tst %i %i\n",10[^] in tst.even, it in tst.odd)
	//printf("tst %i %i\n",10[^] in tst.odd)
	return 0
}
