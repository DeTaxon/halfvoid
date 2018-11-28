
itT := StupidMemoryPool.{16000000}

A := class
{
	x := int
	this := !() -> void
	{
		printf("creating A\n")
	}
	"new" := !() .{@R} -> void^
	{
		printf("alloc type %i\n",R->TypeSize)
		return itT.GetMem(R->TypeSize,R->Align)
	}
}
B := class extend A
{
	y := int[16]
	this := !() -> void
	{
		printf("creating B\n")
	}
	
}


main := !(int argc, char^^ argv) -> int
{
	itT."this"()
	j := new A()
	k := new B()

	k.y[15] = 4
	
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

