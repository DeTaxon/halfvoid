
x := 0
main := !(int argc, char^^ argv) -> int
{
	for 5 
	{
		TSpawnTask(() ==> {
			for i : 100000
			{
				x += 1
				TYield()
			}
		})
	}
	return 0
}
