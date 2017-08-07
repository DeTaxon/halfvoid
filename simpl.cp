printf :=  !(char^ Line,...) -> int declare
puts := !(char^ Line) -> int declare

main := !(int argc,string[] argv) -> int 
{
	x := 7
	puts("{{{Start program}}}")
	printf("Hello %i\n", x + 2) 
	puts("{{{End program}}}")
	return 0
}
