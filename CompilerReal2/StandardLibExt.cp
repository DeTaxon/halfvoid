AddMachine := !(char^ fileName) -> void
{
	
}

RegMachineInit := !() -> void
{
	printf("before\n")
	tst := ParseType(new ObjIndent("WordDetermMachine"))
	printf("hoh %s\n",tst.GetName())
}
