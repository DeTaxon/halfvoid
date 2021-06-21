
x := 0
main := !(int argc, char^^ argv) -> int
{
	r := int[100]
	builtin_setjump(r[0]&)
	printf("wut\n")
	builtin_longjump(r[0]&)
	//for 5 
	//{
	//	TSpawnTask(() ==> {
	//		for i : 100000
	//		{
	//			x += 1
	//			TYield()
	//		}
	//	})
	//}
	return 0
}
