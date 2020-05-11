
Tes := class .{@A[@B]}
{
	$test
	val := !() -> void
	{
		printf("heh %i %i\n",B,$test)
	}
}

RSet.{@A} := type Tes.{int[A]}

main := !() -> int
{
	z := RSet.{13} ; $test
	z.val()
	k := RSet.{13}
	k.val()
	ho := RSet.{12}
	ho.val()
	return 0
}
