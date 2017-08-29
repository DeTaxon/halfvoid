glfwInit := !() -> bool declare
glfwTerminate := !() -> void declare
glfwCreateWindow := !(int x, int y, char^ Title, int A,int B) -> void^ declare
glfwDestroyWindow := !(void^ W) -> void declare 
glfwMakeContextCurrent := !(void^ W) -> void declare
glfwPollEvents := !() -> void declare
glfwWindowShouldClose := !(void^ W) -> bool declare
glfwSetWindowShouldClose := !(void^ W,bool To) -> bool declare
glfwGetTime := !() -> double declare
glfwSwapInterval := !(int Inter) -> void declare
glfwSwapBuffers := !(void^ W) ->void declare

debugCall := type !(int Data,char^ Line)^ -> void
glfwSetErrorCallback := !(debugCall ToCall) -> void declare

keyCall := type !(void^ win, int key, int scancode, int action, int mods)^ -> void
glfwSetKeyCallback := !(void^ win, keyCall ToPress) -> void declare
