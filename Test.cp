printf := !(char^ str, ...) -> int declare
calloc := !(int size) -> void^ declare
malloc := !(int size) -> void^ declare
free := !(void^ point) -> void declare

ve := class
{
	x,y,z := int
}

main := !(int argc, char^^ argv) -> int
{
	waw := ve
	return 0
}
