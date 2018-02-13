printf := !(char^ str, ...) -> int declare
calloc := !(int size) -> void^ declare
free := !(void^ point) -> void declare

pos := class
{
	x,y,z := double
}

main := !(int argc, char^^ argv) -> int
{
	printf("autodetect = %f %f\n", 7deg,7deg)
	return 0
}
"deg" := !(int x)
{
	return x*3.14/180.0
}
