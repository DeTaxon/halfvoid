

main := !(int argc, char^^ argv) -> int
{
	try
	{
		AppendClassTest()
		DeferTest()
		TestQuestion()
		TestBiggerSwitch()
		TestRetQ()
		StringEndTest()
		TestSpaceship()
		TestSpaceshipTuple()
		InsertBeforeTest()
		BestTest()
		TestSetType()
		TaskTest() // Must be last
		printf("all good\n")
	}catch(IException^ e)
	{
		printf("Msg: %s\n",e.Msg())
	}
	return 0
}