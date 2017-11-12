
main := !(int argc,string[] argv) -> int 
{
	Buf := Queue.{Token^}()
	GetTokensFromFile("Test.cp",Buf)
	Ob := TokensToObjects("Test.cp",Buf)
	iter := UniteSkobs(Ob.Down)
	SyntaxCompress(Ob)
	Ob.Print(0)
	if iter != null printf("error iter\n")

	return 0
}

