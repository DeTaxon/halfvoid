
x := 100000
main := !(int argc, char^^ argv) -> int
{
	for r : 5 
	{	
		j := r
		TSpawnTask(() ==> {
			j2 := j
			for i : x
			{
				TYield()
			}
		})
	}
}
