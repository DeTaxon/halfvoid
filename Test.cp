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
	x,y,z := float
	l := vec4f
}
L := class extend J
{
}

"new" := !() . {@R in J} -> void^ 
{
	c := int
	return c&//calloc(newType->TypeSize)->{newType^}
}

main := !(int argc, char^^ argv) -> int
{
	k := new L
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

