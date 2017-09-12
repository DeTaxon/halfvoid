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
	if scancode >= 24 and scancode <= 35 
			Butts[ Line1[scancode - 24] ] = action != 0
	if scancode >= 38 and scancode <= 48
			Butts[ Line2[scancode - 38] ] = action != 0
	if scancode >= 52 and scancode <= 61
			Butts[ Line3[scancode - 52] ] = action != 0
	printf("Key %i mod %i\n",scancode, action)
}

SayError := !(int Data,char^ Line) -> void
{
	printf("Error %i: %s\n",Data,Line)
}


PFN_glCreateProgram := type !()^ -> void 
glCreateProgram := PFN_glCreateProgram

PrintSquare := !(int s) -> void
{
	p := 0.4
	if s == 0 glVertex3f(-p,-p,-p)
	if s == 1 glVertex3f(p,-p,-p)
	if s == 2 glVertex3f(p,p,-p)
	if s == 3 glVertex3f(-p,p,-p)
	if s == 4 glVertex3f(-p,-p,p)
	if s == 5 glVertex3f(p,-p,p)
	if s == 6 glVertex3f(p,p,p)
	if s == 7 glVertex3f(-p,p,p)
}
 
main := !(int argc,string[] argv) -> int 
{
	glfwSetErrorCallback(SayError)
	if not glfwInit() 
	{
		printf("Not loaded\n")
		return 0
	}
	glfwWindowHint(GLFW_DEPTH_BITS,32)
	win = glfwCreateWindow(512,512,"Hi",0,0)
	glfwMakeContextCurrent(win)
	glfwSetKeyCallback(win,keys)
	glfwSwapInterval(1)

	//glCreateProgram = glfwGetProcAddress("glCreateProgram")
	//return 0
	
	Matr := Mat4
	Sec := 0.0

	glViewport(0,0,512,512)
	glClearColor(1.0,.5,0.0,0.0)
	
	Pres := Mat4
	Pres.Persp(1.0,0.1,100.0,75.0)
	Accum := Mat4	
 
	glEnable(GL_DEPTH_TEST)
		
	while not glfwWindowShouldClose(win)
	{
		Sec += 0.004
		Matr = Quant(Sec,Vec3(1.0,1.0,1.0))
		Matr.vec[14] = -1.5
		Accum = Pres
		Accum *= Matr
		//Accum = Matr
		//Accum.Print()
		//printf("\n")
		glLoadMatrixf(Accum.GetP())
		glfwPollEvents()
		glClear(GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT)
		glBegin(GL_QUADS)
		glColor3f(0.0,0.4,0.75)
		PrintSquare(0)
		PrintSquare(1)
		PrintSquare(2)
		PrintSquare(3)
		glColor3f(0.75,0.4,0.0)
		PrintSquare(7)
		PrintSquare(6)
		PrintSquare(5)
		PrintSquare(4)
		glColor3f(0.0,1.0,0.0)
		PrintSquare(0)
		PrintSquare(1)
		PrintSquare(5)
		PrintSquare(4)
		glColor3f(1.0,0.0,1.0)
		PrintSquare(0)
		PrintSquare(3)
		PrintSquare(7)
		PrintSquare(4)
		glColor3f(1.0,1.0,0.0)
		PrintSquare(2)
		PrintSquare(1)
		PrintSquare(5)
		PrintSquare(6)
		glColor3f(0.0,1.0,1.0)
		PrintSquare(3)
		PrintSquare(2)
		PrintSquare(6)
		PrintSquare(7)
		glEnd()
		glfwSwapBuffers(win)
	}
	
	glfwDestroyWindow(win)
	glfwTerminate()
	
	return 0
}

