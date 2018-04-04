Ob := Object^
main := !(int argc,string[] argv) -> int 
{

	CreateStandartTypes()
	CreateBuiltIns()

	PriorityData = new PriorityBag("Priority.pr")
	PriorityData.Opers.Push(":=")

	LexMachine = GenerateMachine(PriorityData.Opers)
	Ob = GetObjectsFromFile("Test.cp")
	

	//Ob.Print(0)
	WorkBag.Push(Ob,State_Start)
	
	WorkWithBag()

	endI := Ob.Down
	if endI != null
		while endI.Right != null 
			endI = endI.Right
	mainFunc := GetItem("main",endI)

	if mainFunc == null ErrorLog.Push("main function not found\n")
	else WorkWithBag()

	Ob.Print(0)
	Ob.TestNodes()

	if ErrorLog.Empty()
	{
		fil := sfile("out2.ll","w")
		StrContainer.PrintGlobal(fil)
		Ob.PrintGlobal(fil)
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

	CleanStrs()
	return 0
}

WorkWithBag := !() -> void
{
	while not WorkBag.IsEmpty()
	{
		prior := WorkBag.GetTopPriority()
		it := WorkBag.Pop()
		it.DoTheWork(prior)
	}
}

GetObjectsFromFile := !(char^ fileName) -> Object^
{
	Buf := Queue.{Token^}()
	GetTokensFromFile(fileName, LexMachine^,Buf)

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

	Ob := TokensToObjects("Test.cp",Buf)
	UniteSkobs(Ob)
	return Ob
}

