printf :=  !(char^ Line,...) -> int declare
sscanf := !(char^ Buf,char^ Format,...) -> int declare
puts := !(char^ Line) -> int declare
malloc := !(int Size) -> void^ declare
free := !(void^ Point) -> void declare
strtof := !(char^ Line) -> float declare
sleep := !(s32 seconds) -> int declare

glfwInit := !() -> bool declare
glfwTerminate := !() -> void declare
glfwCreateWindow := !(int x, int y, char^ Title, int A,int B) -> void^ declare
glfwDestroyWindow := !(void^ W) -> void declare 
glfwMakeContextCurrent := !(void^ W) -> void declare
glfwPollEvents := !() -> void declare
glfwWindowShouldClose := !(void^ W) -> bool declare
glfwSetWindowShouldClose := !(void^ W,bool To) -> bool declare
glfwGetTime := !() -> double declare
glfwSwapInterval := !(int Inter) -> void declare
glfwSwapBuffers := !(void^ W) ->void declare

debugCall := type !(int Data,char^ Line)^ -> void
glfwSetErrorCallback := !(debugCall ToCall) -> void declare

keyCall := type !(void^ win, int key, int scancode, int action, int mods)^ -> void
glfwSetKeyCallback := !(void^ win, keyCall ToPress) -> void declare


glViewport := !(int a,int b,int c,int d) -> void declare
glClearColor := !(float r, float g, float b, float a) ->void declare
glClear := !(int Bits) -> void declare
glBegin := !(int Bits) -> void declare
glEnd := !() -> void declare
glColor3f := !(float r,float g, float b) -> void declare
glVertex2i := !(int x, int y) -> void declare
glVertex3f := !(float x, float y, float z) -> void declare

win := void^

Vec := class
{
	x,y,z := float
	vec := float[3] at x
	this := !() -> void
	{
		x = 0.0
		y = 0.0
		z = 0.0
	}
	size := !() -> float
	{
	      return this*this
	}
	SetX := !(float c) -> void
	{
		x = c
	}
	"+=" := !(Vec ToAdd) -> void
	{
		vec[0] += ToAdd.x
		vec[1] += ToAdd.y
		vec[2] += ToAdd.z
	}
	"*=" := !(float ToAdd) -> void
	{
		x *= ToAdd
		y *= ToAdd
		z *= ToAdd
	}
	"*=" := !(Vec ToAdd) -> void
	{
		vec[0] *= ToAdd.x
		vec[1] *= ToAdd.y
		vec[2] *= ToAdd.z
	}	
	"*" := !(Vec ToAdd) -> float
	{
		Sum := 0.0
		//Sum += vec[^i]*ToAdd.vec[i]
		Sum += x*ToAdd.x
		Sum += y*ToAdd.y
		Sum += z*ToAdd.z
		return Sum
	}
}

Quant := class
{
	Pos,Ang := Vec
}

keys := !(void^ winl, int key, int scancode, int action, int mods) -> void
{
	if scancode == 9
	{
		if action == 1	glfwSetWindowShouldClose(win,true)
	}
	printf("Key %i mod %i\n",scancode, action)
}

SayError := !(int Data,char^ Line) -> void
{
	printf("Error %i: %s\n",Data,Line)
}
 
main := !(int argc,string[] argv) -> int 
{
	Test := Quant
	Plus := Vec

	Plus.x = 0.0
	Plus.vec[0] = 0.0
	Plus.vec[1] = 2.0
	Plus.vec[2] = 0.0

	Test.Pos.SetX(3.0)	
	Test.Pos.vec[0] = 4.0
	Test.Pos.y = 3.0
	Test.Pos.z = 0.0
	
	Te := int[20]
	Te[0] = 12

	printf("Size = %f\n",Test.Pos.size())
	return 0
	
	glfwSetErrorCallback(SayError)
	
	if not glfwInit() 
	{
		printf("Not loaded\n")
		return 0
	}

	win = glfwCreateWindow(512,512,"Hi",0,0)
	glfwMakeContextCurrent(win)
	glfwSetKeyCallback(win,keys)
	glfwSwapInterval(1)

	glViewport(0,0,512,512)
	glClearColor(1.0,.5,0.0,0.0)
	while not glfwWindowShouldClose(win)
	{
		glfwPollEvents()
		glClear(0x04000)
		glBegin(0x0007)
		glColor3f(0.0,0.4,0.75)
		glVertex3f(0.0-0.75,0.0-0.75,0.0)
		glColor3f(0.0,0.4,0.0)
		glVertex3f(0.75,0.0-0.75,0.0)
		glColor3f(0.75,0.4,0.75)
		glVertex3f(0.75,0.75,0.0)
		glColor3f(0.0,0.0,0.0)
		glVertex3f(0.0-0.75,0.75,0.0)
		glEnd()
		glfwSwapBuffers(win)
	}
	
	glfwDestroyWindow(win)
	glfwTerminate()
	
	return 0
}
