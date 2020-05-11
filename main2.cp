
Tes := class .{@A[@B]}
{
	val := !() -> void
	{
		printf("heh %i\n",B)
	}
}

RSet.{@A} := type Tes.{int[A]}

main := !() -> int
{
	z := RSet.{13}
	z.val()
	k := RSet.{13}
	ho := RSet.{12}
	ho.val()
	return 0
}
