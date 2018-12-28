main := !(int argc, char^^ argv) -> int
{
	for i : 1..10
	{
	printf("ww\n")
	defer printf("hallo2\n")
	printf("hallo\n")
	}
	return 0
}

