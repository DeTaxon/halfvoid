#import "lib.cp"
//#import "main.cp"

pls := !() .{@B}
{
	printf("wot %i\n",B)
}

main := !(int argc, char^^ argv) -> int
{
	pls().{1}
	pls().{2}
	return 0
}

