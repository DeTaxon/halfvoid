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
	//printf("Key %i mod %i\n",scancode, action)
}

SayError := !(int Data,char^ Line) -> void
{
	printf("Error %i: %s\n",Data,Line)
}


PFN_glCreateProgram := type !()^ -> void 
glCreateProgram := PFN_glCreateProgram

PrintSquare := !(int s,float p) -> void
{
	if s == 0 glVertex3f(-p,-p,-p)
	if s == 1 glVertex3f(p,-p,-p)
	if s == 2 glVertex3f(p,p,-p)
	if s == 3 glVertex3f(-p,p,-p)
	if s == 4 glVertex3f(-p,-p,p)
	if s == 5 glVertex3f(p,-p,p)
	if s == 6 glVertex3f(p,p,p)
	if s == 7 glVertex3f(-p,p,p)
}

DrawBox := !(float Size) -> void
{
	glBegin(GL_QUADS)
	glColor3f(0.0,0.4,0.75)
	PrintSquare(0,Size)
	PrintSquare(1,Size)
	PrintSquare(2,Size)
	PrintSquare(3,Size)
	glColor3f(0.75,0.4,0.0)
	PrintSquare(7,Size)
	PrintSquare(6,Size)
	PrintSquare(5,Size)
	PrintSquare(4,Size)
	glColor3f(0.0,1.0,0.0)
	PrintSquare(0,Size)
	PrintSquare(1,Size)
	PrintSquare(5,Size)
	PrintSquare(4,Size)
	glColor3f(1.0,0.0,1.0)
	PrintSquare(0,Size)
	PrintSquare(3,Size)
	PrintSquare(7,Size)
	PrintSquare(4,Size)
	glColor3f(1.0,1.0,0.0)
	PrintSquare(2,Size)
	PrintSquare(1,Size)
	PrintSquare(5,Size)
	PrintSquare(6,Size)
	glColor3f(0.0,1.0,1.0)
	PrintSquare(3,Size)
	PrintSquare(2,Size)
	PrintSquare(6,Size)
	PrintSquare(7,Size)
	glEnd()
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
 	Box := Cent
	Box.SetPos(0.0,0.0,-1.5)
	Box2 := Cent
	Box2.SetAng(0.0,Vec3(1.0,1.0,1.0))
	Box2.SetPos(0.0,0.0,0.7)
	BoxSum := Cent
	C := Camera()
	
	glEnable(GL_DEPTH_TEST)
		
	while not glfwWindowShouldClose(win)
	{
		glfwPollEvents()
		glClear(GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT)
		Sec += 0.004
		
		if Butts['r'] C.Mouse(-0.012,0.0)
		if Butts['f'] C.Mouse( 0.012,0.0)
		if Butts['e'] C.Mouse(0.0, 0.012)
		if Butts['q'] C.Mouse(0.0,-0.012)
		if Butts['w'] C.Local(Vec3(0.0,0.0,-0.1))
		if Butts['s'] C.Local(Vec3(0.0,0.0, 0.1))
		if Butts['d'] C.Local(Vec3( 0.1,0.0,0.0))
		if Butts['a'] C.Local(Vec3(-0.1,0.0,0.0))
		C.Update()

		//C.Set(BoxSum)		
		BoxSum = C.Dot
	
		Box.SetAng(Sec,Vec3(0.0,1.0,0.0))
		BoxSum *= Box
		Matr = BoxSum
		Accum = Pres
		Accum *= Matr
		glLoadMatrixf(Accum.row[0].x&)
		DrawBox(0.1)
		
		Box2.SetAng(Sec*2.0,Vec3(0.0,0.0,1.0))
		BoxSum *= Box2
		Matr = BoxSum
		Accum = Pres
		Accum *= Matr
		glLoadMatrixf(Accum.row[0].x&)
		DrawBox(0.3)


		glfwSwapBuffers(win)
	}
	
	glfwDestroyWindow(win)
	glfwTerminate()
	
	return 0
}

