printf := !(char^ str, ...) -> int declare
calloc := !(int size) -> void^ declare
free := !(void^ point) -> void declare

max := !(a,b)
{
	if a > b return a
	return b
}

main := !(int argc, char^^ argv) -> int
{
	max(3,4)
	return 0
}
