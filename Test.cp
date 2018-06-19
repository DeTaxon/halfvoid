#import "lib.cp"
//#import "main.cp"
#import "MappedFileLinux.cp"
#import "glfw.cp"

main := !(int argc, char^^ argv) -> int
{
	glfwInit()

	win := glfwCreateWindow(500,500,"Hi!",null,null)

	while not glfwWindowShouldClose(win)
	{
		glfwPollEvents()
		
	}

	return 0
}

