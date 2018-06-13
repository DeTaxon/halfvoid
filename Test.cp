#import "lib.cp"
//#import "main.cp"

sh := !(int a) . {int^} -> void
{
	printf("lol %i\n",a)
}
main := !(int argc, char^^ argv) -> int
{
	sh(3).{
		int^
		}
	return 0
}

