#import "lib.cp"
//#import "main.cp"

sh := !(a) -> void
{
	printf("lol %i\n",a)
}
main := !(int argc, char^^ argv) -> int
{
	sh(3).{2}
	sh(3).{2}
	return 0
}

