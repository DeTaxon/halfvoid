
main := !() -> int
{
	x := 13
	y := () ==> [x&] {
		k := 7
		while true
		{
			printf("hop %i\n",x)
			x += 23
			yield void
			printf("hep %i\n",x)
			x += 23
			yield void
			printf("hor %i\n",x)
			x += 23
			yield void
		}
	}
	y()
	z := y.Capture()
	z()
	w := z.Capture()
	w()
	printf("heh %i\n",x)

	//a := 0
	//b := a&
	//r := () ==>  
	//{
	//	r2 := () ==> {
	//		x := 0
	//		while true
	//		{
	//			b^ += 1
	//			printf("heh %i\n",b^)
	//			yield void
	//		}
	//	}
	//	while true
	//	{
	//		r2()
	//		yield void
	//	}
	//}
	//r()
	//r()
	//r()
	//r()
	return 0
}
