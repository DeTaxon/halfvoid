printf :=  !(char^ Line,...) -> int declare
puts := !(char^ Line) -> int declare
malloc := !(int Size) -> void^ declare
free := !(void^ Point) -> void declare

main := !(int argc,string[] argv) -> int 
{
	i := u32^
	j := u8^
	j = i
	puts("{{{Start program}}}")
	printf("Hello %p\n" , i ) 
	puts("{{{End program}}}")
	return 0
}
