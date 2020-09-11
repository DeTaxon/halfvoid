main := !(int argc, char^^ argv) -> int
{
	arr := AVLMap.{int,int}()
	arr[1] = 3
	arr[4] = 13
	c := arr.values.Get()
	printf("v %i\n",c[^])
	return 0
}
