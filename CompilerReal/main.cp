
GlobalItems := MapCont.{string,Object^}

main := !(int argc,string[] argv) -> int 
{
	Buf := Queue.{Token^}()

	Pris := PriorityBag("Priority.pr")
	iterW := Pris.Opers.Start

	while iterW != null
	{
		printf("oper %s\n",iterW.Data)
		iterW = iterW.Next
	}
	return 0 

	GetTokensFromFile("Test.cp",Buf)
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

