
main := !(int argc, char^^ argv) -> int
{
	test := AVLMap.{string,int}()

	test["3"] = 157
	test["6"] = 367
	test["9"] = -1
	printf("wut %i\n",test["3"])
	printf("wut %i\n",test["6"])
	printf("wut %i\n",test["9"])

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

