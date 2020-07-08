
main := !(int argc, char^^ argv) -> int
{
	lex := LexBuilder()
	lex.ApplyReg("[0-9,]+")
	isNum := lex.GenerateMachine()
	
	d := WordParser
	strr := "123 wow 5,6"
	d.ReadText(isNum&,strr,StrSize(strr),(a,b,c) ==>
	{
		printf("%s\n",strr[b..c])
	})

	return 0
}
