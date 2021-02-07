regMachines := AVLMap.{char^,u8^}

AddMachine := !(char^ fileName) -> void
{
	mInfo := MappedFile(fileName)
	defer mInfo.Close()
	cpy := new u8[mInfo.Size()]
	memcpy(cpy->{void^},mInfo.Get(),mInfo.Size())
	regMachines[StrCopy(Path(fileName).Name())] = cpy
}

machineType := Type^
RegMachineInit := !() -> void
{
	printf("before\n")
	machineType = ParseType(new ObjIndent("WordDetermMachine"))
	printf("hoh %s\n",machineType.GetName())
}
PrintRegMachines := !(sfile f) -> void
{
	for it,key : regMachines
	{
		f << "@"<< key <<" = global " << machineType.GetName() << " zeroinitializer\n"
	}
}
