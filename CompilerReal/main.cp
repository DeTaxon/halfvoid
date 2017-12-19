
GlobalItems := MapCont.{string,Object^}

main := !(int argc,string[] argv) -> int 
{
	Buf := Queue.{Token^}()

	Pris := PriorityBag("Priority.pr")
	//iterW := Pris.Opers.Start
	//if iterW == null printf("empty\n")
	//while iterW != null
	//{
	//	printf("oper <%s>\n",iterW.Data)
	//	iterW = iterW.Next
	//}
	Mach := GenerateMachine(Pris.Opers)
	GetTokensFromFile("Test.cp", Mach,Buf)
	return 0 
	Ob := TokensToObjects("Test.cp",Buf)
	iter := UniteSkobs(Ob.Down)
	SyntaxCompress(Ob)

	if iter != null printf("error iter\n")
	else
	{
		//item.ParseMeta()
		CollectParams(Ob)
	}	
	Ob.Print(0)

	return 0
}

