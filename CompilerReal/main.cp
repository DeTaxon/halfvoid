
main := !(int argc,string[] argv) -> int 
{
	CreateStandartTypes()

	PriorityData = new PriorityBag("Priority.pr")
	PriorityData.Opers.Push(":=")

	LexMachine = GenerateMachine(PriorityData.Opers)
	Ob := GetObjectsFromFile("Test.cp")


	WorkBag.Push(Ob,State_Start)

	while not WorkBag.IsEmpty()
	{
		prior := WorkBag.GetTopPriority()
		it := WorkBag.Pop()
		it.DoTheWork(prior)
	}

	Ob.Print(0)
	
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


	return 0
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

