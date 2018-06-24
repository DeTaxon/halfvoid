#import "lib.cp"
//#import "main.cp"
//#import "MappedFileLinux.cp"
//#import "glfw.cp"
//#import "gl.cp"
//
//glClearColor := !(float,float,float,float)^ -> void
//glClear := !(int)^ -> void

H := !().{@J}  -> void
{
	x := J
	printf("good\n")
}


main := !(int argc, char^^ argv) -> int
{
	H().{int}
	//glfwInit()

	//win := glfwCreateWindow(500,500,"Hi!",null,null)
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
	//	
	//}

	return 0
}

