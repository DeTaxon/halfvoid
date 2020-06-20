main := !(int argc, char^^ argv) -> int
{
	itRep := new vRepo
	itRep.Init(".")
	itRep.AddZipRoot("./test.zip")
	intrRepoBrowser = itRep
	
	ldt := Library("glfw3.dll")
	//ldt := Library("glgl.dll")
	init := ldt.Get("glfwInit")->{!()^->void}
	crtW := ldt.Get("glfwCreateWindow")->{!(int,int,char^,void^,void^)^->void^}
	contx := ldt.Get("glfwMakeContextCurrent")->{!(void^)^->void}
	shlC := ldt.Get("glfwWindowShouldClose")->{!(void^)^->bool}
	Poll := ldt.Get("glfwPollEvents")->{!(void^)^->void}

	init()
	wind := crtW(100,100,"wow",null,null)
	contx(wind)
	while not shlC(wind)
	{
		Poll(wind)
	}
	return 0
}
