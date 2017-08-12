printf :=  !(char^ Line,...) -> int declare
puts := !(char^ Line) -> int declare
malloc := !(int Size) -> void^ declare
free := !(void^ Point) -> void declare
sleep := !(s32 seconds) -> int declare

glfwInit := !() -> bool declare
glfwTerminate := !() -> void declare
glfwCreateWindow := !(int x, int y, char^ Title, int A,int B) -> void^ declare
glfwDestroyWindow := !(void^ W) -> void declare 
glfwMakeContextCurrent := !(void^ W) -> void declare

debugCall := type !(int Data,char^ Line)^ -> void
glfwSetCallback := !(debugCall ToCall) -> void declare

SayError := !(int Data, char^ Line) -> void
{
	printf("Error %i: %s",Data,Line)
}

main := !(int argc,string[] argv) -> int 
{
	
	//i := 0
	//do
	//{
	//	printf("i = %i\n",i)
	//	i = i - 1
	//}while i > 0
	//puts("{{{Start program}}}")
	//printf("Hello %i\n" , i ) 
	//puts("{{{End program}}}")
	//free(i)

	if glfwInit() puts("Work") else puts("False")
	win := glfwCreateWindow(500,500,"Hi",0,0)
	printf("w = %p\n",win)
	glfwMakeContextCurrent(win)
	sleep(5)
	glfwDestroyWindow(win)
	glfwTerminate()
	
	return 0
}
