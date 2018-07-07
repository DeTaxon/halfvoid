#import "lib.cp"
//#import "main.cp"
//#import "MappedFile.cp"
//#import "glfw.cp"
//#import "gl.cp"
//#import "Model.cp"
#import "arrs.cp"

//glClearColor := !(float,float,float,float)^ -> void
//glClear := !(int)^ -> void
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


main := !(int argc, char^^ argv) -> int
{
	
	//m := Model()
	//m.LoadFromPLY("HiResBox.ply")

	c := Queue.{int}()
	c.Push(6)
	c.Push(10)
	c.Push(4)

	printf("wut %i\n",c[0])

	return 0

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
	//glClearColor(1.0f,0.5f,0.0f,1.0f)

	//while not glfwWindowShouldClose(win)
	//{
	//	glfwPollEvents()

	//	glClear(GL_COLOR_BUFFER_BIT)

	//	glfwSwapBuffers(win)
	//}

	//return 0
}

