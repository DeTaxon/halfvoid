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
		for 3 vec[it] = 0.0
	}
	size := !() -> float
	{
	      return this*this
	}
	Set := !(float a,float b,float c) -> void
	{
		x = a
		y = b
		z = c
	}
	"+=" := !(Vec ToAdd) -> void
	{
		for 3 vec[it] += ToAdd.vec[it]
	}
	"*=" := !(float ToAdd) -> void
	{
		for 3 vec[it] *= ToAdd
	}
	"*=" := !(Vec ToAdd) -> void
	{
		for it,i : vec it *= ToAdd.vec[i]
	}	
	"*" := !(Vec ToAdd) -> float
	{
		Sum := 0.0
		//Sum += vec[^i]*ToAdd.vec[i]
		//for it,i vec += it*ToAdd.vec[i]
		for 3 Sum += vec[it]*ToAdd.vec[it]
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

GlobalPos := Vec
 
main := !(int argc,string[] argv) -> int 
{
	Te := int[5]
	for i,j : Te i = (j + 4)*2
	for Te printf("i = %i\n",it)	
`
	for i : 4
		for j : 4 
			printf("%i %i\n",i,j)
	Test := Quant

	Test.Pos.Set(4.0,3.0,0.0)
	GlobalPos.Set(0.0,3.0,0.0)
	
	Te[0] =  5 * (7 - 13)

	printf("Size = %f\n",Test.Pos * GlobalPos)
	printf("Size = %i\n",Te[0])
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
		glVertex3f(-0.75,-0.75,0.0)
		glColor3f(0.0,0.4,0.0)
		glVertex3f(0.75,-0.75,0.0)
		glColor3f(0.75,0.4,0.75)
		glVertex3f(0.75,0.75,0.0)
		glColor3f(0.0,0.0,0.0)
		glVertex3f(-0.75,0.75,0.0)
		glEnd()
		glfwSwapBuffers(win)
	}
	
	glfwDestroyWindow(win)
	glfwTerminate()
	
	return 0
}

