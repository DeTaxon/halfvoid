
main := !(int argc, char^^ argv) -> int
{
	defer printf("!\n")
	while true
	{
		printf("hello\n")
		defer printf("world\n")
		break	
	}
	printf("problem\n")
	return 0
}
