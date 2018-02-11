printf := !(char^ str, ...) -> int declare
calloc := !(int size) -> void^ declare
free := !(void^ point) -> void declare



main := !(int argc, char^^ argv) -> int
{
	printf("autodetect = %f %f\n", 7deg,7deg)
	AsVoid()
	return 0
}
AsVoid := !() -> bool
{
	printf("void\n")
	return 4
}
fun := !()
{
	return 7.0
}
"deg" := !(int x)
{
	return x*3.14/180.0
}
