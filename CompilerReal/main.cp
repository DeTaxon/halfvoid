
GlobalItems := MapCont.{string,Object^}

main := !(int argc,string[] argv) -> int 
{
	Buf := Queue.{Token^}()
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

