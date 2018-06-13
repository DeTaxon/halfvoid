#import "lib.cp"
//#import "main.cp"

"->{}" := !(int x) . {char^} -> string
{
	return "5"
}

main := !(int argc, char^^ argv) -> int
{
	x := 6->{string}
	printf("st %s\n",x)
	return 0
}

