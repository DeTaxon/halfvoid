main := !(int argc,char^^ argv) -> int 
{
	ReturnName = "ToRet"
	StrContainer = new StringContainer()
	WorkBag."this"()
	//BuiltInFuncs."this"()
	//ObjectsPool."this"()
	//GlobalStrs = ""
	printWork := false
	CTT = new CreateTupleTemplate
	TCT = new TupleConstructorTemplate

	targetFiles := Queue.{string}()
	targetObjects := Queue.{Object^}()
	outputFile := "out.ll"

	codeSp := Queue.{Pair.{int,string}}()
	zipSp := Queue.{Pair.{int,string}}()

	targetPlatform := "posix"

	emitTree := false

	i := 1
	while  i < argc
	{
		switch argv[i]
		{
		case "--ZipGlue"
			ZipConCat(argv[i+1],argv[i+2],argv[i+3])
			return 0
		case "--ZipAppend"
			zL := List.{char^}() ; $temp
			for j : (i+2)..(argc-1)
				zL << argv[j]
			ZipAppend(argv[i+1],zL)
			return 0
		case "--rname"
			ReturnName = argv[i+1]
			i += 1
		case "--fatstr"
			UseFatString = true
		case  "--tree"
			emitTree = true
		case "--cci"
			InCC = argv[i+1]
			i += 1
		case "--cco"
			OutCC = argv[i+1]
			i += 1
		case "-p"
			i++
			targetPlatform = argv[i]
		case "-o"
			outputFile = argv[i+1]
			i += 1
		case "-g"
			DebugMode = true	
			cuId = GetNewId()
		case "--vk"
			i += 1
			p := Path(argv[i])
			if p.IsExist(){
				Modules.Push(new ModuleVulkan(argv[i]))
			}else{
				printf("--vk module does not exist\n")
				return 0
			}
		case "--work"
			printWork = true
		case void
			if StrSize(argv[i]) >= 3 and argv[i][0..2] == "-C"
			{
				itPri := StrToInt(argv[i][2..0])
				i++

				itStb := ""sbt << argv[i] << ".cp"
				tmp1 := itStb.Str() ; $temp
				for newItm : Wildcard(tmp1)
				{
					codeSp.Emplace(itPri,StrCopy(newItm.itStr))
				}
				itStb << "." << targetPlatform
				tmp1 = itStb.Str() ; $temp
				for newItm : Wildcard(tmp1)
					codeSp.Emplace(itPri,StrCopy(newItm.itStr))

			}else{
				if StrSize(argv[i]) >= 3 and argv[i][0..2] == "-Z"
				{
					itPri := StrToInt(argv[i][2..0])
					itP := Path(argv[i+1])
					if not itP.IsExist()
					{
						printf("zip file %s not found\n",argv[i+1])
						return 0
					}
					zipSp.Emplace(itPri,argv[i+1])
					i++
				}else{
					targetFiles.Push(argv[i])
				}
			}
		}
		i += 1
	}

	if targetFiles.Size() != 1
	{
		printf("ERROR: must be only 1 file\n")
		for targetFiles
		{
			printf("input file %s\n",it)
		}
		return 0
	}

	CreateStandartTypes()
	CreateBuiltIns()

	GBoolTrue = new ObjBool(true)
	GBoolFalse = new ObjBool(false)

	selfFile := new ZipFile() ; $temp

	loadedLex := false

	if selfFile.AnalizeFile(argv[0])
	{
		defer selfFile.DecUser()
		itMach := selfFile.GetFile("Mach.m")

		itPri := selfFile.GetFile("Priority.pr")
		if itPri == null
			itPri = selfFile.GetFile("Priority2.pr")

		if itPri !=null
		{
			priPtr := itPri.Map()
			PriorityData = new PriorityBag(priPtr->{char^},itPri.realSize)
			itPri.Unmap()
		}
		if itMach != null
		{
			loadedLex = true
			mPoint := itMach.Map()
			LoadLexMachine(mPoint->{char^},itMach.realSize)
			itMach.Unmap()
		}
	}
	if not loadedLex
	{
		printf("lexical machine loaded from outside\n")
		itMach := MappedFile("./Mach.m")
		LoadLexMachine(itMach.point,itMach.Size())
		itMach.Close()
	}

	if PriorityData == null
	{
		printf("priority loaded from outside\n")
		prFile := MappedFile("./Priority.pr")
		defer prFile.Close()
		PriorityData = new PriorityBag(prFile.point,prFile.Size())
	}
	PutConstString(":=")
	PutConstString("for")
	PutConstString("if")
	PutConstString("extern")
	PutConstString("virtual")
	PutConstString("class")
	PutConstString("return")
	PutConstString("self_return")

	PriorityData.Opers.Push(":=")
	PriorityData.Opers.Push("=>")
	PriorityData.Opers.Push("-->")
	PriorityData.Opers.Push("extern")
	PriorityData.Opers.Push("at")
	PriorityData.Opers.Push("defer")
	PriorityData.Opers.Push("type")
	PriorityData.Opers.Push("virtual")

	OpersTree << PriorityData.Opers[^]

	for targetFiles
	{
		ItPath := Path(it)
		resA := ItPath.IsExist()
		resB := not resA
		if resB
		{
			printf("file %s does not exist\n",it)
			return 0
		}
	}
	targetObjects.Push(LoadFile(Path(targetFiles[^])))

	for it : codeSp
	{
		fL := LoadFile(Path(it.second))
		if fL == null
		{
			printf("file %s does not exist\n",it.second)
		}
		fLibSp := ref CodeSpaces[it.first]
		fL.cs = fLibSp&
		fLibSp.codeLibs.Push(fL)
		FilesInSpace.Insert(fL)

	}
	for it : zipSp
	{
		newFiles := new Queue.{void^} ; $temp
		LoadZipFile(Path(it.second),newFiles^)

		fLibSp := ref CodeSpaces[it.first]
		for fils : newFiles^
		{
			zFile := fils->{BoxFile^}
			zFile.cs = fLibSp&
			fLibSp.codeLibs.Push(zFile)
			FilesInSpace.Insert(zFile)
		}
	}

	Ob := targetObjects[0]

	Modules[^].InitModule()
	
	WorkBag.Push(Ob,State_Start)
	
	WorkWithBag(printWork)

	if not DeferInit2()
	{
		for ErrorLog printf(it)
		printf("internal error, no defer manager\n")
		return -1
	}
	if not ExceptionInit()
	{
		for ErrorLog printf(it)
		printf("internal error, no exception functions\n")
		return -1
	}

	endI := Ob.Down
	if endI != null
		while endI.Right != null 
			endI = endI.Right
	mainFunc := GetItem("main",endI)

	if mainFunc != null
		WorkBag.Push(mainFunc.Down,State_Start)

	if mainFunc == null ErrorLog.Push("main function not found\n")
	else WorkWithBag(printWork)

	//Ob.Print(0)
	Ob.TestNodes()

	if ErrorLog.Empty()
	{
		printf("good to go\n")
		for PostFuncs it.PostCreate()

		fil := sfile(outputFile,"w")
		fil << "declare float     @llvm.pow.f32(float  %Val, float %Power)\n"
		fil << "declare double    @llvm.pow.f64(double %Val, double %Power)\n"
		fil << "declare i32 @llvm.eh.sjlj.setjmp(i8* %abc) #1\n"
		fil << "declare void @llvm.eh.sjlj.longjmp(i8* %abc) #3\n"
		fil << "declare i32 @setjmp(i8* %abc) #1\n"
		fil << "declare void @longjmp(i8* %abc,i32 %ty) #3\n"
		fil << "declare i8* @llvm.eh.sjlj.lsda() #0\n"
		fil << "declare void @llvm.debugtrap() #0\n"
		fil << "declare i8* @llvm.frameaddress(i32 %asd) #2\n"
		fil << "declare i8* @llvm.stacksave() #1\n"
		fil << "declare i8* @llvm.stackrestore(i8* %abc) #0\n"
		fil << "declare float @llvm.experimental.vector.reduce.fadd.f32.v4f32(float %acc, <4 x float> %a)\n"
		fil << "declare float @llvm.experimental.vector.reduce.fadd.f32.v3f32(float %acc, <3 x float> %a)\n"
		fil << "declare float @llvm.experimental.vector.reduce.fadd.f32.v2f32(float %acc, <2 x float> %a)\n"
		fil << "declare float @llvm.experimental.vector.reduce.v2.fadd.f32.v4f32(float %start_value, <4 x float> %a)"
		fil << "declare float @llvm.experimental.vector.reduce.v2.fadd.f32.v3f32(float %start_value, <3 x float> %a)"
		fil << "declare float @llvm.experimental.vector.reduce.v2.fadd.f32.v2f32(float %start_value, <2 x float> %a)"
		fil << "target triple=\"x86_64-pc-linux-gnu\"\n"
		fil << "attributes #0 = { nounwind \"target-cpu\"=\"x86-64\"  }\n"
		fil << "attributes #1 = { nounwind }\n"
		fil << "attributes #2 = { nounwind readnone}\n"
		fil << "attributes #3 = { nounwind noreturn}\n"
		fil << "%OpaqType = type {i1}\n"
		fil << "%Vec4f = type <4 x float>\n"
		fil << "%Vec3f = type <3 x float>\n"
		fil << "%Vec2f = type <2 x float>\n"
		StrContainer.PrintGlobal(fil)

		Classes[^].PrintStruct(fil)
		PrintTuples(fil)
		fil << GlobalStrs[^]
		PrintTuplesFuncs(fil)


		for wutt : Files
		{
			FlushTempMemory()
			if emitTree wutt.Print(0)
			wutt.PrintGlobal(fil)
		}
		for wutt : ZipFiles
		{
			FlushTempMemory()
			if emitTree wutt.Print(0)
			wutt.PrintGlobal(fil)
		}
		if DebugMode
		{
			PrintDebugMeta(fil)
		}
		fil.close()
	}else
	{
		if emitTree for Files it.Print(0)
		for ErrorLog printf(it)
		return -1
	}
	//CleanStrs() shiet
	//if not ErrorLog.Empty() return -1
	printf("Created func types %i\n",GetFuncTypeCount())
	//PrintMemUse()
	return 0
}

workIter := int
WorkWithBag := !(bool printW) -> void
{
	while (not WorkBag.IsEmpty()) and ErrorLog.Empty()
	{
		FlushTempMemory()
		prior := WorkBag.GetTopPriority()
		it := WorkBag.Pop()
		workingOnObject = it

		if printW
		{
			printf("itWork %i %s\n",workIter,it.GetValue())
			if it.Line != null {
				printf("at %s %i\n",it.Line.inFile.itStr,it.Line.LinePos)
				//it.Print(0)
				}
			workIter += 1
		}

		it.DoTheWork(prior)
	}
}
