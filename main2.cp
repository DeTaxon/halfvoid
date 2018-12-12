

main := !(int argc, char^^ argv) -> int
{	
	c := new int[5]
	for i : 5 c[i] = 2*i
	for c printf("it %i\n",it)
	return 0
}

