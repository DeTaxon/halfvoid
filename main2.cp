

main := !(int argc, char^^ argv) -> int
{
	work := TPushWork(() ==> {
		for i : 500
		{
			TSleep(1)
		}
	})
	work.Await()
}
