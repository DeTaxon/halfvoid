win := void^

Butts := bool[256]
Line1 := "qwertyuiop[]"
Line2 := "asdfghjkl;'"
Line3 := "zxcvbnm,./"

keys := !(void^ winl, int key, int scancode, int action, int mods) -> void
{
	if scancode == 9
	{
		if action == 1	glfwSetWindowShouldClose(win,true)
	}
	if scancode >= 24 
		if scancode <= 35
			Butts[ Line1[scancode - 24] ] = action != 0
	if scancode >= 38 
		if scancode <= 48
			Butts[ Line2[scancode - 38] ] = action != 0
	if scancode >= 52 
		if scancode <= 61
			Butts[ Line3[scancode - 52] ] = action != 0
	//printf("Key %i mod %i\n",scancode, action)
}

SayError := !(int Data,char^ Line) -> void
{
	printf("Error %i: %s\n",Data,Line)
}


PFN_glCreateProgram := type !()^ -> void 
glCreateProgram := PFN_glCreateProgram
 
main := !(int argc,string[] argv) -> int 
{
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

	//glCreateProgram = glfwGetProcAddress("glCreateProgram")
	//return 0
	
	Matr := Mat4
	Ang := Vec3
	Sec := 0.0
	BAng := Quant
	
	Ang.Set(0.0,0.0,1.0)
	glViewport(0,0,512,512)
	glClearColor(1.0,.5,0.0,0.0)
	
	while not glfwWindowShouldClose(win)
	{
		Sec += 0.001
		BAng.SetAng(Sec,Ang)
		Matr = BAng
		//printf("\n")
		glLoadMatrixf(Matr.GetP())
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

