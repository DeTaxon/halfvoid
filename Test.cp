printf := !(char^ str, ...) -> int declare
calloc := !(int size) -> void^ declare
free := !(void^ point) -> void declare



main := !(int argc, char^^ argv) -> int
{
	printf("autodetect = %f %f\n", fun(),7deg)
	return 0
}
fun := !()
{
	return 7.0
}
"deg" := !(int x)
{
	return x*3.14/180.0
}
