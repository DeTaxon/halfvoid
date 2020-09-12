max2 := !(@T a,args...) -> #best(T,args) 
{
	mx := a->{#best(T,args)}
	(mx = args > mx ?: args : mx)...
	return mx
}


main := !(int argc, char^^ argv) -> int
{
	k := max2(1,23,26.7,-12)
	printf("wow %f\n",k)
	return 0
}
