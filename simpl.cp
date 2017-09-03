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


//PFN_glCreateProgram := type !()^ -> void 
//glCreateProgram := PFN_glCreateProgram
 
main := !(int argc,string[] argv) -> int 
{
	glfwSetErrorCallback(SayError)
	//glCreateProgram = glfwGetProcAddress("glCreateProgram")
	//if glCreateProgram != null printf("Hey\n")
	//glCreateProgram()	
	if not glfwInit() 
	{
		printf("Not loaded\n")
		return 0
	}

	win = glfwCreateWindow(512,512,"Hi",0,0)
	glfwMakeContextCurrent(win)
	glfwSetKeyCallback(win,keys)
	glfwSwapInterval(1)
	
	Matr := Mat4
	Ang := Vec3
	Sec := 0.0
	BAng := Quant
	
	Ang.Set(0.0,1.0,0.0)
	glViewport(0,0,512,512)
	glClearColor(1.0,.5,0.0,0.0)
	while not glfwWindowShouldClose(win)
	{
		Sec += 0.001
		BAng.SetAng(Sec,Ang)
		BAng.Print()
		Matr = BAng
		Matr.Print()	
		printf("\n")
		glLoadMatrixf(Matr.GetP())
		//glRotatef(Sec,0.0,0.0,1.0)
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

