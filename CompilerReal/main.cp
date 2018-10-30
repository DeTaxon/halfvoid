
main := !(int argc,char^^ argv) -> int 
{
	CTT = new CreateTupleTemplate()

	targetFiles := Queue.{string}()
	targetObjects := Queue.{Object^}()
	outputFile := "out.ll"

	forcedFiles := Queue.{string}()

	emitTree := false

	i := 1
	while  i < argc
	{
		if argv[i] == "--tree" {
			emitTree = true
		}else{

			if argv[i] == "-f"
			{
				forcedFiles.Push(argv[i+1])
				i += 1
			}else{
				if argv[i] == "-o"
				{
					outputFile = argv[i+1]
					i += 1
				}else{
					targetFiles.Push(argv[i])
				}
			}
		}
		i += 1
	}

	if targetFiles.Size() != 1
	{
		printf("WARN:only one target file requared\n")
		targetFiles.Push("main2.cp")
		forcedFiles.Push("Libs/lib.cp")
	}

	CreateStandartTypes()
	CreateBuiltIns()

	PriorityData = new PriorityBag("Priority.pr")
	PriorityData.Opers.Push(":=")
	PriorityData.Opers.Push("=>")
	PriorityData.Opers.Push("extern")
	PriorityData.Opers.Push("at")
	PriorityData.Opers.Push("defer")

	LexMachine = GenerateMachine(PriorityData.Opers)

	for i : targetFiles.Size()
	{
		ItPath := Path(targetFiles[i])
		resA := ItPath.IsExist()
		resB := not resA
		if resB
		{
			printf("file %s does not exist\n",targetFiles[i])
			return 0
		}
	}
	for i : targetFiles.Size()
	{
		targetObjects.Push(LoadFile(Path(targetFiles[i])))
	}
	for i : forcedFiles.Size()
	{
		fL := LoadFile(Path(forcedFiles[i]))
		ForcedLibs.Push(fL)
	}

	Ob := targetObjects[0]
	
	WorkBag.Push(Ob,State_Start)
	
	WorkWithBag()

	endI := Ob.Down
	if endI != null
		while endI.Right != null 
			endI = endI.Right
	mainFunc := GetItem("main",endI)

	if mainFunc != null 
		WorkBag.Push(mainFunc.Down,State_Start)

	if mainFunc == null ErrorLog.Push("main function not found\n")
	else WorkWithBag()

	//Ob.Print(0)
	Ob.TestNodes()

	if ErrorLog.Empty()
	{
		printf("good to go\n")
		iterTR := PostFuncs.Start
		while iterTR != null
		{
			iterTR.Data.PostCreate()
			iterTR = iterTR.Next
		}

		fil := sfile(outputFile,"w")
		fil << GlobalStrs
		fil << "declare float     @llvm.pow.f32(float  %Val, float %Power)\n"
		fil << "declare double    @llvm.pow.f64(double %Val, double %Power)\n"
		fil << "declare float @llvm.experimental.vector.reduce.fadd.f32.v4f32(float %acc, <4 x float> %a)\n"
		StrContainer.PrintGlobal(fil)

		for Classes.Size() 
			Classes[it].PrintStruct(fil)

		PrintTuples(fil)

		wutt := Files.Start
		while wutt != null
		{
			if emitTree wutt.Data^.Print(0)
			wutt.Data^.PrintGlobal(fil)
			wutt = wutt.Next
		}
		fil.close()
	}else
	{
		iterTr := Files.Start
		while iterTr != null
		{
			if emitTree iterTr.Data^.Print(0)
			iterTr = iterTr.Next
		}
		ite := ErrorLog.Start
		while ite != null
		{
			printf(ite.Data)
			ite = ite.Next
		}
	}
	//CleanStrs() shiet
	//if not ErrorLog.Empty() return -1
	return 0
}


workIter := int
WorkWithBag := !() -> void
{
	while not WorkBag.IsEmpty() and ErrorLog.Empty()
	{
		prior := WorkBag.GetTopPriority()
		it := WorkBag.Pop()
		//printf("working on %p %s\n",it,it.GetValue())
		//printf("itWork %i %s %p\n",workIter,it.GetValue(),it)
		//if it.Line != null {
		//	printf("at %s %i\n",it.Line.inFile.itStr,it.Line.LinePos)
		//	//it.Print(0)
		//	}
		//workIter += 1
		it.DoTheWork(prior)
	}
}

GetObjectsFromFile := !(Path fileName) -> Object^
{
	Buf := Queue.{Token^}()
	if not GetTokensFromFile(fileName, LexMachine^,Buf)
		return null

	iterC := Buf.Start
	while iterC != null
	{
		iterG := PriorityData.Opers.Start
		while iterG != null
		{
			if iterG.Data == iterC.Data.Buff{
				iterC.Data.Id = 10
			}
			iterG = iterG.Next
		}
		iterC = iterC.Next
	}

	Ob := TokensToObjects(fileName,Buf)
	UniteSkobs(Ob)
	return Ob
}

