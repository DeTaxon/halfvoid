printf :=  !(char^ Line,...) -> int declare
puts := !(char^ Line) -> int declare



main := !(int argc,string[] argv) -> int 
{
	z := "%s %i\n"
	i := u8
	puts("{{{Start program}}}")
	printf(z , "Hello" , i) 
	puts("{{{End program}}}")
	return 0
}
