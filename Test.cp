#import "lib.cp"
//#import "main.cp"

Y := class
{
	o := int
	"[]" := !(int x) -> int
	{
		return x + 3
	}
}

main := !(int argc, char^^ argv) -> int
{
	c := Y
	printf("wut %i\n",c[10 + 2])
	return 0
}

