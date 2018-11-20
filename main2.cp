

te := type Queue.{int}

main := !(int argc, char^^ argv) -> int
{
	r := te()
	c := te()
	r.Push(1)
	r.Push(2)
	c.Push(3)
	
	for r printf("wow %i\n",it)
	for c printf("wow %i\n",it)
	
	return 0
}

//A := class
//{
//	x := int
//	SetX := !(int f) -> void
//	{
//		x = f
//	}
//	Waw := !() -> void
//	{
//		SetX(3)
//	}
//}
//
//B := class extend A
//{
//}
//
//C := class extend B
//{
//	Wow := !(int u) -> void
//	{
//		SetX(u)
//	}
//}
//main := !(int argc, char^^ argv) -> int
//{
//	R := C
//	R.Wow(5)
//	printf("wut %i\n",R.x)
//	return 0
//}
//

