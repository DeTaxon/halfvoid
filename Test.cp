printf := !(char^ str, ...) -> int declare

wut := !() -> double
{
	return 0.0
}


main := !(int argc, char^^ argv) -> int
{
	j := 3.0 - wut()
	printf("j = %f\n",j)
	return 0
}
