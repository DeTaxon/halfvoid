
Tes := class .{@A[@B]}
{
	$test
	val := !() -> void
	{
		printf("heh %i %i\n",B,$test)
	}
}

Anoter := class .{@A}
{
	item := A.{int[13]}
}

Wut.{@B} := type Anoter.{B}

main := !() -> int
{
	z := Anoter.{Tes}
	z.item.val()
	return 0
}
