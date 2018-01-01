
GlobalItems := MapCont.{string,Object^}

main := !(int argc,string[] argv) -> int 
{
	fil := sfile("out2.ll","w")
	fil <= "a " <= " maybe " <= "\n"
	fil.close()
	return 0
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

	Test := GetBoxFunc(Ob.Down.Down)
	if Test == null printf("no\n")
	else{
		printf("parsed\n")
		printf("\n")
	}
	return 0
}

