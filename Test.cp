//#import "main.cp"
//#import "MappedFile.cp"
//#import "glfw.cp"
//#import "gl.cp"
//#import "Model.cp"
//#import "arrs.cp"
//#import "math.cp"
//#import "xcb-lib.cp"
#import "RegExpBuilder.cp"
#import "Bitset.cp"
#import "Pair.cp"
#import "WordParser.cp"


//glClearColor := !(float,float,float,float)^ -> void
//glClear := !(int)^ -> void
//
//glBegin := !(int)^ -> void
//glVertex3fv := !(float^)^ -> void
//glColor3fv := !(float^)^ -> void
//glEnd := !()^ -> void
//
//glLoadIdentity := !()^ -> void
//gluPerspective := !(double,double,double,double)^ -> void
//glRotatef := !(float,float,float,float)^ -> void
//glTranslatef := !(float,float,float)^ -> void
//glMatrixMode := !(int)^ -> void
//glLoadMatrixf := !(float^)^ -> void
//
//keys := bool[256]
//
//error_callback := !(int error,char^ descp) -> void
//{
//	printf("error: %s\n",descp)
//}
//
//key_input  := !(void^ win, int key, int scancode, int action , int mods) -> void
//{
//	if action != GLFW_RELEASE and action != GLFW_PRESS return void
//	res := action == GLFW_RELEASE	
//	
//	if key in GLFW_KEY_A..GLFW_KEY_Z keys[key - GLFW_KEY_A + 'a'] = action
//	if key in GLFW_KEY_0..GLFW_KEY_9 keys[key - GLFW_KEY_0 + '0'] = action
//}
//
//camera_t := class 
//{
//	pos := vec4f
//	angX,angY := float
//
//	this := !() -> void
//	{
//		pos = vec4f(0.0f,0.0f,0.0f,1.0f)
//		angX = 0.0f
//		angY = 0.0f
//	}
//
//	CheckMove := !(float diff) -> void
//	{
//		ang := quantfAt(0.0f,1.0f,0.0f,angY) <*> quantfAt(1.0f,0.0f,0.0f,angX)
//		cons := 0.005f
//		atVec := ang * vec4f(0.0f,0.0f,cons*diff,1.0f)
//		right := ang * vec4f(cons*diff,0.0f,0.0f,1.0f)
//		
//		if keys['w']	{ pos += atVec }
//		if keys['s']	{ pos -= atVec }
//		if keys['a']	{ pos += right}
//		if keys['d']	{ pos -= right}
//		if keys['q']	{ angY += cons}
//		if keys['e']	{ angY -= cons}
//		if keys['r']	{ angX += cons}
//		if keys['f']	{ angX -= cons}
//
//		if angY > 2.0pi angY -= 2.0pi
//		if angY < 0.0f angY += 2.0pi
//		if angX > 2.0pi angX -= 2.0pi
//		if angX < 0.0f angX += 2.0pi
//	}
//	GetCent := !() -> centf
//	{
//		ang := quantfAt(0.0f,1.0f,0.0f,angY) <*> quantfAt(1.0f,0.0f,0.0f,angX)
//		revAng := quantf(-ang.x,-ang.y,-ang.z,ang.w)
//		revPos := vec4f(-pos.x,-pos.y,-pos.z,pos.w)
//
//		ToRet.ang = ang
//		ToRet.pos = revAng*pos
//	}
//}

main := !(int argc, char^^ argv) -> int
{
	//c := dlopen("libxcb.so",2)
	//printf("wut %p\n",c)
	//if c == null return 0
	//b := dlsym(c,"xcb_generate_id")
	//printf("wuut %p\n",b) 
	//dlclose(c)

	if argc != 2 return 1
	B := LexBuilder()
	//B.ApplyReg("[0-9]+(.[0-9]+)?")
	B.ApplyReg(argv[1])
	te := WordParser
	C := B.GenerateMachine()
	//te.itCore = C&
	//te.itCode.IsEndNode = C.IsEndNode
	//for i : 256 te.itCode.CharToGo[i] = C.CharToGo[i]
	wow := "123 dhsdfhbadfv 123.567"
	te.ReadText(C&,wow,23, (x y z) =>
	{
		printf("value %i %i %i\n",x,y,z)
	})

	return 0
	
	//M := DetMachine

	//return 0

	//m := Model()
	//m.LoadFromPLY("HiResBox.ply")

	//glfwInit()
	//glfwSetErrorCallback(error_callback)

	//win := glfwCreateWindow(500,500,"Hi!",null,null)

	//glfwSetKeyCallback(win,key_input)

	//glfwMakeContextCurrent(win)
	//glfwSwapInterval(1)
	//
	//glClearColor = glfwGetProcAddress("glClearColor")
	//glClear = glfwGetProcAddress("glClear")

	//glBegin = glfwGetProcAddress("glBegin") 
	//glVertex3fv = glfwGetProcAddress("glVertex3fv")
	//glColor3fv = glfwGetProcAddress("glColor3fv")
	//glEnd 	= glfwGetProcAddress("glEnd")

	//glLoadIdentity = glfwGetProcAddress("glLoadIdentity")
	//gluPerspective = glfwGetProcAddress("gluPerspective")
	//glRotatef = glfwGetProcAddress("glRotatef")
	//glTranslatef = glfwGetProcAddress("glTranslatef")
	//glMatrixMode = glfwGetProcAddress("glMatrixMode")
	//glLoadMatrixf = glfwGetProcAddress("glLoadMatrixf")

	//glClearColor(1.0f,0.5f,0.0f,1.0f)
	//

	//fov := 45deg
	//fF := 1.0f / tanf(fov*0.5f)
	//zFar := 200.0f
	//zNear := 1.5f
	//diff := zNear - zFar
	//sum := zNear + zFar
	//aspect := 1.0f

	//matrProj := float[16]
	//for c : 16 matrProj[c] = 0.0f
	//matrProj[0] = fF / aspect
	//matrProj[5] = fF
	//matrProj[10] = sum / diff
	//matrProj[11] = 2.0f*zFar*zNear / diff
	//matrProj[14] = -1.0f
	//
	//c := camera_t()
	//matr := float[16]

	//glMatrixMode(GL_PROJECTION)
	//glLoadMatrixf(matrProj)
	//glMatrixMode(GL_MODELVIEW)

	//while not glfwWindowShouldClose(win)
	//{
	//	glfwPollEvents()
	//	
	//	c.CheckMove(1.0f)

	//	glClear(GL_COLOR_BUFFER_BIT)
	//	c.GetCent().FillMatr(matr)
	//	glLoadIdentity()
	//	glLoadMatrixf(matr[0]&)
	//	//glTranslatef(vecPos.x,vecPos.y,vecPos.z)

	//	glBegin(GL_TRIANGLES)
	//	for i : m.inds->len
	//	{
	//		nowInd := m.inds[i]
	//		nowInd *= 8
	//		glColor3fv(m.verts[nowInd+3]&)
	//		glVertex3fv(m.verts[nowInd]&)
	//	}
	//	glEnd()

	//	glfwSwapBuffers(win)
	//}

	//return 0
}

