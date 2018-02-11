printf := !(char^ str, ...) -> int declare
calloc := !(int size) -> void^ declare
free := !(void^ point) -> void declare



main := !(int argc, char^^ argv) -> int
{
	printf("autodetect = %f\n", fun(6))
	return 0
}
fun := !(int x)
{
	return 7.0
}
"deg" := !(int x) -> double
{
	return x*3.14/180.0
}
