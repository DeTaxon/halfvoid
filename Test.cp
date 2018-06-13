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
"twice" := !(double x) -> float
{
	return x*2.0
}

main := !(int argc, char^^ argv) -> int
{
	j := W
	printf("wow %s %f %f %f\n",j->{string},2.0pi,1pi,0.5pi)
	return 0
}

