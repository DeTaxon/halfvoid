printf :=  !(char^ Line,...) -> int declare
puts := !(char^ Line) -> int declare
malloc := !(int Size) -> void^ declare
free := !(void^ Point) -> void declare
//sleep := !(u32 seconds) -> int declare

main := !(int argc,string[] argv) -> int 
{
	i := 5
	while i 
	{
		printf("i = %i\n",i)
		i = i - 1
	}
	puts("{{{Start program}}}")
	printf("Hello %i\n" , i ) 
	puts("{{{End program}}}")
	//free(i)
	return 0
}
