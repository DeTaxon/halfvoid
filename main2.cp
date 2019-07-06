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
	k := t ; $a
	k << 1 << 2 << 3
	k.C(1).C(2).C(3)
	return 0
}

