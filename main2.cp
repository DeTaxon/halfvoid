printf := !(char^ a,...) -> void declare

t := class
{
	x := int
	"<<" := !(int y) .{} self_return
	{
		if $a
		{
			printf("a\n")
		}else{
			printf("b\n")
		}
	}
	C := !(int y) .{} self_return
	{
		if $a
		{
			printf("a\n")
		}else{
			printf("b\n")
		}
	}

}

main := !(int argc, char^^ argv) -> int
{
	printf("heh\x0A")
	return 0
}

