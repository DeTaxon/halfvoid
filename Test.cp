printf := !(char^ str, ...) -> int declare
calloc := !(int size) -> void^ declare
free := !(void^ point) -> void declare

main := !(int argc, char^^ argv) -> int
{
	printf("wit %i %f %i\n", max(3,4), max(1.4,1.66), max(6,1) )
	return 0
}

max := !(a,b)
{
	if a > b return a
	return b
}
