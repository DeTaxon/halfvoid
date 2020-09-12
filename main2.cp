max2 := !(@T a,args...) -> T
{
	mx := a
	(mx = args > mx ?: args : mx)...
	return mx
}


main := !(int argc, char^^ argv) -> int
{
	a := 3.0
	b := 4
	k := ref (true ?: a : b)
	printf("a b %f %i %f\n",a ,b,k)
	return 0
}
