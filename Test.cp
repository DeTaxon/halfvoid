#import "lib.cp"
//#import "main.cp"

W := class 
{
	y := int
	"->{}" := !() . {string} -> string
	{
		return "www"
	}
}

main := !(int argc, char^^ argv) -> int
{
	j := W
	printf("wow %s\n",j->{string})
	return 0
}

