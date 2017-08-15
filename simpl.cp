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
glfwPollEvents := !() -> void declare

debugCall := type !(int Data,char^ Line)^ -> void
glfwSetErrorCallback := !(debugCall ToCall) -> void declare

keyCall := type !(void^ win, int key, int scancode, int action, int mods)^ -> void
glfwSetKeyCallback := !(void^ win, keyCall ToPress) -> void declare

SayError := !(int Data, char^ Line) -> void
{
	printf("Error %i: %s\n",Data,Line)
}

keys := !(void^ win, int key, int scancode, int action, int mods) -> void
{
	printf("Key %i\n",scancode)
}

main := !(int argc,string[] argv) -> int 
{
	printf("x = %f\n", 1.3)

	return 0

	glfwSetErrorCallback(SayError)
	
	if not glfwInit() 
	{
		printf("Not loaded\n")
		return 0
	}

	win := glfwCreateWindow(500,500,"Hi",0,0)
	//glfwMakeContextCurrent(win)
	glfwSetKeyCallback(win,keys)

	i := int
	i = 10
	while i 
	{
		glfwPollEvents()
		sleep(1)
		i = i - 1
	}
	
	glfwDestroyWindow(win)
	glfwTerminate()
	
	return 0
}
