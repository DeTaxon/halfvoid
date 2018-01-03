
GlobalItems := MapCont.{string,Object^}

main := !(int argc,string[] argv) -> int 
{
	CreateStandartTypes()
	Buf := Queue.{Token^}()

	Pris := PriorityBag("Priority.pr")
	//iterW := Pris.Opers.Start
	//if iterW == null printf("empty\n")
	//while iterW != null
	//{
	//	printf("oper <%s>\n",iterW.Data)
	//	iterW = iterW.Next
	//}
	Pris.Opers.Push(":=")
	Mach := GenerateMachine(Pris.Opers)
	GetTokensFromFile("Test.cp", Mach,Buf)
	iterC := Buf.Start
	while iterC != null
	{
		iterG := Pris.Opers.Start
		while iterG != null
		{
			if iterG.Data == iterC.Data.Buff{
				iterC.Data.Id = 10
			}
			iterG = iterG.Next
		}
		//printf("token %i <%s>\n",iterC.Data.Id,iterC.Data.Buff)
		iterC = iterC.Next
	}
	//return 0 
	Ob := TokensToObjects("Test.cp",Buf)
	iter := UniteSkobs(Ob.Down)

	SyntaxCompress(Ob,Pris)
	//Ob.Print(0)


	if iter != null printf("error iter\n")
	else
	{
		//item.ParseMeta()
		CollectParams(Ob)
	}	
	Ob.Print(0)

	Test := GetUse(Ob.Down.Down)
	if Test == null printf("no\n")
	else{
		if (Test.IsInvalid) printf("almost\n")
		else
		{
			printf("parsed\n")
			fil := sfile("out2.ll","w")
			StrContainer.PrintGlobal(fil)
			Test.PrintGlobal(fil)
			fil.close()
		}
	}
	return 0
}

