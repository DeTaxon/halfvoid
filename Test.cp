printf := !(char^ str, ...) -> int declare
calloc := !(int size) -> void^ declare
free := !(void^ point) -> void declare

"deg" := !(int x)
{
	return x*3.14/180.0
}


main := !(int argc, char^^ argv) -> int
{
	return 0
}
