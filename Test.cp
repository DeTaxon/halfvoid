#import "lib.cp"
//#import "main.cp"
//#import "MappedFile.cp"
//#import "glfw.cp"
//#import "gl.cp"
//#import "Model.cp"
//#import "arrs.cp"

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
//key_input  := !(void^ win, int key, int scancode, int action , int mods) ->void
//{
//	if action != GLFW_RELEASE and action != GLFW_PRESS return void
//	res := action == GLFW_RELEASE	
//	
//	if key in GLFW_KEY_A..GLFW_KEY_Z keys[key - GLFW_KEY_A + 'a'] = action
//	if key in GLFW_KEY_0..GLFW_KEY_9 keys[key - GLFW_KEY_0 + '0'] = action
//}

"<*>" := !(quantf a,quantf b) -> quantf
{
	res := quantf
	x := (a * b.wzyx) <+> vec4f(1.0f,1.0f,-1.0f,1.0f)
	return res
}

main := !(int argc, char^^ argv) -> int
{
	x := quantf(1.0f,1.0f,1.0f,1.0f)
	y := quantf(1.0f,1.0f,1.0f,1.0f)
	x <*> y


	return 0
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
	//
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
	//vecPos := vec4f(0.0f,0.0f,0.0f,0.0f)

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

	//glMatrixMode(GL_PROJECTION)
	//glLoadMatrixf(matrProj)
	//glMatrixMode(GL_MODELVIEW)

	//while not glfwWindowShouldClose(win)
	//{
	//	glfwPollEvents()

	//	if keys['w']
	//	{
	//		vecPos += vec4f(0.0f,0.0f,0.005f,0.0f)
	//	}
	//	if keys['s']
	//	{
	//		vecPos -= vec4f(0.0f,0.0f,0.005f,0.0f)
	//	}
	//	if keys['a']
	//	{
	//		vecPos += vec4f(0.005f,0.0f,0.0f,0.0f)
	//	}
	//	if keys['d']
	//	{
	//		vecPos -= vec4f(0.005f,0.0f,0.0f,0.0f)
	//	}

	//	glClear(GL_COLOR_BUFFER_BIT)

	//	glLoadIdentity()
	//	glTranslatef(vecPos.x,vecPos.y,vecPos.z)

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

