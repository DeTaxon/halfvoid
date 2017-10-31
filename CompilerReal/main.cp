
A := class 
{
	printWow := virtual !() -> void
	{
		printf("no\n")
	}
}
B := class extend A
{
	printWow := virtual !() -> void
	{
		printf("yes\n")
	}
}

main := !(int argc,string[] argv) -> int 
{
	Buf := Queue.{Token^}()
	GetTokensFromFile("Test.cp",Buf)
	Ob := TokensToObjects("Test.cp",Buf)

	iter := Ob.Down
	while iter != null
	{
		iter.Print()
		iter = iter.Right
		printf("\n")
	}
	P := A^
	R := new B
	P = R
	P.printWow()
	return 0
}

