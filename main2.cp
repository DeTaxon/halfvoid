C := class extend TGCObject
{
	Destroy := virtual !() -> void
	{
		printf("yep\n")
	}
	next := C^
}

main := !(int argc, char^^ argv) -> int
{
	r := List.{C}()
	r.Emplace()
	return 0
}
