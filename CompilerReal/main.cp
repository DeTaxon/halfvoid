Ob := Object^
main := !(int argc,string[] argv) -> int 
{

	CreateStandartTypes()
	CreateBuiltIns()

	PriorityData = new PriorityBag("Priority.pr")
	PriorityData.Opers.Push(":=")
	PriorityData.Opers.Push("extern")
	PriorityData.Opers.Push("at")

	LexMachine = GenerateMachine(PriorityData.Opers)
	Ob = LoadFile("Test.cp")
	
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
		iterTR := PostFuncs.Start
		while iterTR != null
		{
			iterTR.Data.PostCreate()
			iterTR = iterTR.Next
		}

		fil := sfile("out2.ll","w")
		fil << GlobalStrs
		fil << "declare float     @llvm.pow.f32(float  %Val, float %Power)\n"
		fil << "declare double    @llvm.pow.f64(double %Val, double %Power)\n"
		fil << "declare float @llvm.experimental.vector.reduce.fadd.f32.v4f32(float %acc, <4 x float> %a)\n"
		StrContainer.PrintGlobal(fil)

		for Classes.Size() Classes[it].PrintStruct(fil)

		wutt := Files.Start
		while wutt != null
		{
			//wutt.Data^.Print(0)
			wutt.Data^.PrintGlobal(fil)
			wutt = wutt.Next
		}
		fil.close()
	}else
	{
		ite := ErrorLog.Start
		while ite != null
		{
			printf(ite.Data)
			ite = ite.Next
		}
	}
	//CleanStrs() shiet
	return 0
}

WorkWithBag := !() -> void
{
	while not WorkBag.IsEmpty() and ErrorLog.Empty()
	{
		prior := WorkBag.GetTopPriority()
		it := WorkBag.Pop()
		it.DoTheWork(prior)
	}
}

GetObjectsFromFile := !(char^ fileName) -> Object^
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

