#import "lib.cp"
//#import "main.cp"
//#import "MappedFileLinux.cp"
//#import "glfw.cp"
//#import "gl.cp"
//
//glClearColor := !(float,float,float,float)^ -> void
//glClear := !(int)^ -> void

J := class
{
	x,y,z := int
	 G := virtual !() -> void
	 {
	 	printf("wow\n")
	 }
}
L := class extend J
{
	this := !(int h) -> void
	{
		x = h
	}
}

"new" := !() . {@R in J} -> void^ 
{
	return malloc(R->TypeSize)
}


main := !(int argc, char^^ argv) -> int
{
	k := new L(5)
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

