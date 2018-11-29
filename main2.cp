


main := !(int argc, char^^ argv) -> int
{
	hi := HybridQueue.{int,16}()

	for 32 hi.Push(it)

	for hi printf("wow %i\n",it)
	
	return 0
}

