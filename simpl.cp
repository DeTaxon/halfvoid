printf :=  !(char^ Line,...) -> int declare
puts := !(char^ Line) -> int declare
malloc := !(int Size) -> void^ declare
free := !(void^ Point) -> void declare
//sleep := !(u32 seconds) -> int declare

main := !(int argc,string[] argv) -> int 
{
	i := int^
	i = malloc(32)
	i[5] = 5
	puts("{{{Start program}}}")
	printf("Hello %p %i\n" , i ,i[5] ) 
	puts("{{{End program}}}")
	free(i)
	return 0
}
