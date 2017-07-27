printf :=  !(char^ Line,...) -> int declare
puts := !(char^ LIne) -> int declare

main := !(int argc,char[]^ argv) -> void 
{
	puts("{{{Start program}}}")
	printf("Hello %s\n","world!")
	puts("{{{End program}}}")
	return 0
}
