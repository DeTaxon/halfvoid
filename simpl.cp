win := void^

Butts := bool[256]

keys := !(void^ winl, int key, int scancode, int action, int mods) -> void
{
	if scancode == 9 and action == 1	glfwSetWindowShouldClose(win,true)
	if scancode in 24..35 	Butts[ "qwertyuiop[]"[scancode - 24] ] = action != 0
	if scancode in 38..48	Butts[ "asdfghjkl;'" [scancode - 38] ] = action != 0
	if scancode in 52..61	Butts[ "zxcvbnm,./"  [scancode - 52] ] = action != 0
	//printf("Key %i mod %i\n",scancode, action)
}

SayError := !(int Data,char^ Line) -> void
{
	printf("Error %i: %s\n",Data,Line)
}


glCreateProgram := !()^ -> void

PrintSquare := !(int s,float p) -> void
{
	if s == 0 glVertex3f(-p,-p,-p)
	if s == 1 glVertex3f( p,-p,-p)
	if s == 2 glVertex3f( p, p,-p)
	if s == 3 glVertex3f(-p, p,-p)
	if s == 4 glVertex3f(-p,-p, p)
	if s == 5 glVertex3f( p,-p, p)
	if s == 6 glVertex3f( p, p, p)
	if s == 7 glVertex3f(-p, p, p)
}

DrawBox := !(float Size) -> void
{
	glBegin(GL_QUADS)
	glColor3f(0.0,0.4,0.75)
	for ![0,1,2,3]	PrintSquare(it,Size)
	glColor3f(0.75,0.4,0.0)
	for ![7,6,5,4]	PrintSquare(it,Size)
	glColor3f(0.0,1.0,0.0)
	for ![0,1,5,4]	PrintSquare(it,Size)
	glColor3f(1.0,0.0,1.0)
	for ![0,3,7,4]	PrintSquare(it,Size)
	glColor3f(1.0,1.0,0.0)
	for ![2,1,5,6]	PrintSquare(it,Size)
	glColor3f(0.0,1.0,1.0)
	for ![3,2,6,7]	PrintSquare(it,Size)
	glEnd()
}

test := !(char^ this) -> void{
	printf("ico\n")
}
test := !(char^ NonThis) -> void{
	printf("boco\n")
}

main := !(int argc,string[] argv) -> int 
{
	//R := Queue.{int}
	//R.Push(1)
	//R.Push(-6)
	//R.Push(7)
	//for i : R printf("%i\n",i)
	
	GLInfo := false
	Bo := Model("TestBox.ply")

	for argc if argv[it] == "GL" GLInfo = true
	//if argv[^] == "GL" GLInfo = true
	// GLInfo = "GL" in argv

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

	glCreateProgram = glfwGetProcAddress("glCreateProgram")

	if GLInfo
	{
		printf("extensions :\n")
		h := glGetString(GL_EXTENSIONS)

		Exts := Queue.{char^}()
		DivideStr(h,' ',Exts)
		
		while Exts.NotEmpty()
		{
			L := Exts.Pop()
			printf("extension %s\n",L)
			free(L)
		}
		printf("version : %s\n",glGetString(GL_VERSION))
		printf("vendor  : %s\n",glGetString(GL_VENDOR))
		printf("renderer: %s\n",glGetString(GL_RENDERER))
		printf("shader  : %s\n",glGetString(GL_SHADING_LANGUAGE_VERSION))
		return 0
	}
	
	Matr := Mat4
	Sec := 0.0
	
	glViewport(0, 0, 512, 512)
	glClearColor(1.0, 0.5, 0.0, 0.0)
	
	Pres := Mat4
	Pres.Persp(1.0, 0.1, 100.0, 75.0)
	Accum := Mat4
 	Box := Cent
	Box.SetPos(0.0, 0.0, -3.5)
	Box2 := Cent
	Box2.SetAng(0.0,Vec3(1.0, 1.0, 1.0))
	Box2.SetPos(0.0, 0.0, 2.7)
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

		if Butts['w'] C.Local(Vec3( 0.0, 0.0,-0.1))
		if Butts['s'] C.Local(Vec3( 0.0, 0.0, 0.1))
		if Butts['d'] C.Local(Vec3( 0.1, 0.0, 0.0))
		if Butts['a'] C.Local(Vec3(-0.1, 0.0, 0.0))

		C.Update()

		BoxSum = C.Dot
	
		Box.SetAng(Sec,Vec3(0.0,1.0,0.0))
		BoxSum *= Box
		Matr = C.Dot*Box
		Accum = Pres
		Accum *= Matr
		glLoadMatrixf(Accum.vec)
		DrawBox(0.7)
		
		Box2.SetAng(Sec*2.0,Vec3(0.0,0.0,1.0))
		BoxSum *= Box2
		Matr = BoxSum
		Accum = Pres
		Accum *= Matr
		glLoadMatrixf(Accum.vec)
		Bo.draw()

		glfwSwapBuffers(win)
	}
	
	glfwDestroyWindow(win)
	glfwTerminate()
	
	return 0
}

