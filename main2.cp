


main := !(int argc, char^^ argv) -> int
{
	for i : Wildcard("./*.cp")
	{
		printf("stuf %s\n", i.itStr)
	}
	return 0
}

