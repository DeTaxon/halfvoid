#import "VulkanCore.cp"


main := !(int argc, char^^ argv) -> int
{
	InitVulkan()
	defer DestroyVulkan()
	return 0

	glfwInit()
	defer glfwTerminate()

	glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API)
	win := glfwCreateWindow(640,480,"Hi again",null,null)

	//glfwMakeContextCurrent(win)

	while not glfwWindowShouldClose(win)
	{
		//glClear(GL_COLOR_BUFFER_BIT)
		//glfwSwapBuffers(win)
		glfwPollEvents()
	}
		
	return 0

}

