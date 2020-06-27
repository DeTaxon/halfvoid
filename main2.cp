
main := !(int argc, char^^ argv) -> int
{
	try
	{
		printf("good\n")
	}catch(IException^ exp)
	{
		printf("bad %s\n",exp.Msg())
	}
	return 0
}
