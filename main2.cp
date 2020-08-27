
bag := class
{
	//odd := fake
	//{
	//	"in" := !(int x) -> bool
	//	{
	//		return x anb_b 1
	//	}
	//}
	"in" := !(int x) -> void
	{
		printf("hop %i\n",x)
	}
}


main := !(int argc, char^^ argv) -> int
{
	tst := bag
	13 in tst
	return 0
}
