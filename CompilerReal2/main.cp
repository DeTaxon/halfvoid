
clibModule := CLibModule

main := !(int argc,char^^ argv) -> int 
{
	gRepo.Init(".")
	ReturnName = "result"
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
	outputFile := "out.ll"->{char^}

	codeSp := Queue.{Pair.{int,string}}()
	zipSp := Queue.{Pair.{int,string}}()

	fileSuffixes := List.{char^}()
	fileSuffixes << ".cp"

	dirChecks := List.{Tuple.{char^,int}}()

	emitTree := false

	Modules.Push(clibModule&)

	i := 1
	while  i < argc
	{
		switch argv[i]
		{
		case "--task_gs"
			EnableGSTask = true
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
			itNm := ".cp."sbt + argv[i]
			fileSuffixes.Push(itNm.Str())
			
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
		case "--clib"
			i += 1
			clibModule.AddCLib(gRepo.GetFile(argv[i]),false)
		case "--work"
			printWork = true
		case void
			if StrSize(argv[i]) >= 3 and argv[i][0..2] == "-C"
			{
				itPri := StrToInt(argv[i][2..0])
				i++
				dirChecks.Emplace(argv[i][0]&,itPri)

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

	for toDir : dirChecks
	{
		itStb := ""sbt << toDir.0
		tmp1 := itStb.Str() ; $temp
		for k : tmp1 if k == '$' k = '*'
		anyFileName := char^()
		for suf : fileSuffixes
		{
			tmp2Pre := ""sbt + tmp1  + suf
			tmp2 := tmp2Pre.Str() ; $temp
			for newItm : Wildcard(tmp2)
			{
				flName := StrCopy(newItm.itStr)
				codeSp.Emplace(toDir.1,flName)
				anyFileName = flName
			}
		}

		machCheck := ""sbt + tmp1 + ".stm"
		for machs : Wildcard(machCheck)
		{
			AddMachine(machs.itStr)
		}
			
		//if DebugMode and anyFileName != null
		//{
		//	pa := Path(anyFileName).FolderName()
		//	nwName := ""sbt + pa + "/pretty.py" <-
		//	if Path(nwName).IsExist()
		//	{
		//		prettyFile := fopen(nwName,"r")	
		//		miniBuffer := char[1024]
		//		while feof(prettyFile) == 0
		//		{
		//			fgets(miniBuffer,1024,prettyFile)
		//			DebugPretty.Push(StrCopy(miniBuffer))
		//		}
		//		fclose(prettyFile)
		//	}
		//}
	}
	InfiniteCodeSpace = CodeSpaces[2000000]&

	CreateStandartTypes()
	CreateBuiltIns()

	GBoolTrue = new ObjBool(true)
	GBoolFalse = new ObjBool(false)
	

	for it : fileSuffixes
	{
		if strlen(it) < 4 continue
		GlobalAttributes[it[4..0].Str()] = GBoolTrue
	}
	if DebugMode 
	{
		GlobalAttributes["debug"] = GBoolTrue
	}

	MacroRestart = new Object

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

	PriorityData.Opers.Insert(":=")
	PriorityData.Opers.Insert("=>")
	PriorityData.Opers.Insert("-->")
	PriorityData.Opers.Insert("extern")
	PriorityData.Opers.Insert("at")
	PriorityData.Opers.Insert("defer")
	PriorityData.Opers.Insert("type")
	PriorityData.Opers.Insert("virtual")

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
		LoadZipFile(Path(it.second),newFiles^,fileSuffixes)

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

	TaskInit()
	Modules[^].InitModule()
	
	WorkBag.Push(Ob,State_Start)
	
	WorkWithBag(printWork)

	if not DeferInit2()
	{
		for ErrorLog printf(it)
		printf("warning: internal error, no defer manager\n")
	}
	if not ExceptionInit()
	{
		for ErrorLog printf(it)
		printf("warning: internal error, no exception functions\n")
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
	//Ob.TestNodes()

	if not ErrorLog.Empty(){
		if emitTree for Files it.Print(0)
		for ErrorLog printf(it)
		return -1
	}

	//fType := clibModule.CheckFuncTypeString(StringSpan("void"))
	//nFunc := new BoxFuncBodyFromString(fType," x := 13")
	//WorkWithBag(printWork)
	//nFunc.Print(0)

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
	fil << "declare i8* @llvm.frameaddress.p0i8(i32 %asd) #2\n"
	fil << "declare i8* @llvm.stacksave() #1\n"
	fil << "declare i8* @llvm.stackrestore(i8* %abc) #0\n"
	fil << "declare float @llvm.experimental.vector.reduce.fadd.f32.v4f32(float %acc, <4 x float> %a)\n"
	fil << "declare float @llvm.experimental.vector.reduce.fadd.f32.v3f32(float %acc, <3 x float> %a)\n"
	fil << "declare float @llvm.experimental.vector.reduce.fadd.f32.v2f32(float %acc, <2 x float> %a)\n"
	fil << "declare float @llvm.experimental.vector.reduce.v2.fadd.f32.v4f32(float %start_value, <4 x float> %a)\n"
	fil << "declare float @llvm.experimental.vector.reduce.v2.fadd.f32.v3f32(float %start_value, <3 x float> %a)\n"
	fil << "declare float @llvm.experimental.vector.reduce.v2.fadd.f32.v2f32(float %start_value, <2 x float> %a)\n"
	//fil << "target triple=\"x86_64-pc-linux-gnu\"\n"
	fil << "attributes #0 = { nounwind \"target-cpu\"=\"x86-64\"  }\n"
	fil << "attributes #1 = { nounwind }\n"
	fil << "attributes #2 = { nounwind readnone}\n"
	fil << "attributes #3 = { nounwind noreturn}\n"
	fil << "%OpaqType = type {i1}\n"
	fil << "%Vec4f = type <4 x float>\n"
	fil << "%Vec3f = type <3 x float>\n"
	fil << "%Vec2f = type <2 x float>\n"

	fil << "declare void @llvm.va_start(i8* %a)\n"
	fil << "declare void @llvm.va_end(i8* %a)\n"
	fil << "declare void @llvm.va_copy(i8* %a,i8* %b)\n"
	PrintLambdaGlobal(fil)
	StrContainer.PrintGlobal(fil)
	GlobalDataBuiltins[^].PrintGlobal(fil)
	TaskPrint(fil)
	//nFunc.PrintGlobal(fil)

	Classes[^].PrintStruct(fil)
	PrintTuples(fil)
	fil << GlobalStrs[^]
	Modules[^].PrintGlobal(fil)
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
		PrintDebRefs(fil)
	}
	fil.close()

	//if not ErrorLog.Empty() return -1
	printf("Created func types %i\n",GetFuncTypeCount())
	//PrintMemUse()
	return 0
}

workIter := int
WorkWithBag := !(bool printW) -> void
{
	try
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
	}catch(IException^ e)
	{
		return void
	}
}
