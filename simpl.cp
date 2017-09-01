win := void^


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

Node := class !{T}
{
	Data := T
	Next := Node.{T}^
}

Stack := class !{T}
{
	Start := Node.{T}^
	Push := !(T a) -> void
	{
		if Start == null {
		} else {
		}
	}
}
 
main := !(int argc,string[] argv) -> int 
{
	Ints := Stack.{int}
	Floats := Stack.{float}
	Ints.Start = null
	Ints.Push(1)
	return 0
	Te := new Vec(3.0,2.0,1.0)
	Te^.x = 20.0
	printf("i = %f\n",Te.y)
	free(Te)
	//return 0
	//#for i,j : Te i = (j + 4)*2
	//#for Te printf("i = %i\n",it)	

	for i : 4
		for j : 4 
			printf("%i %i\n",i,j)
	Test := Quant

	Test.Pos.Set(4.0,3.0,0.0)
	GlobalPos.Set(0.0,3.0,0.0)
	

	printf("Size = %f\n",Test.Pos * GlobalPos)
	//return 0
	
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
		glClear(GL_COLOR_BUFFER_BIT)
		glBegin(GL_QUADS)
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

