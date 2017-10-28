
main := !(int argc,string[] argv) -> int 
{
	Buf := Queue.{Token^}()
	GetTokensFromFile("Test.cp",Buf)
	while Buf.NotEmpty()
	{
		Tok := Buf.Pop()
		printf("Token %s %i\n",Tok.Buff,Tok.Id)
		free(Tok)
	}	
	return 0
}

