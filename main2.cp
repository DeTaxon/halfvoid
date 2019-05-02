
hah := !(args...) -> void
{
	printf("hello %i %i\n",args...)
}

main := !(int argc, char^^ argv) -> int
{
	hah(1,2)
	return 0
}

