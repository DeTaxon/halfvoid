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
	machineType = ParseType(new ObjIndent("WordDetermMachine"))
}
PrintRegMachines := !(TIOStream f) -> void
{
	for it,key : regMachines
	{
		states := it[0]&->{int^}^
		lines := it[4]&->{int^}^
		chList := it[8]&

		IsEndNode := chList[256]&->{int^}
		f << "@" << key << "IsEndNode = global [" << states + 1 << " x i32] "
		f << "[ i32 " << states 
		for i : states
		{
			f << ", i32 " << IsEndNode[i]
		}
		f << "]\n"

		iter := IsEndNode[states]&
		for i : states
		{
			f << "@" << key << "line" << i<< " = global [" << lines + 1 << " x i32] [ i32 " << lines
			for j : lines
			{
				f << ", i32 " << iter[j]
			}
			f << "]\n"
			iter = iter[lines]&
		}

		f << "%" << key << "alltyp = type {i32,i32,[" << states << " x i32*]}\n"
		f << "@" << key << "all = global %"<<key<<"alltyp {i32 " << states << ",i32 " << states<< ",[" << states << " x i32*][\n"
		for i : states
		{
			if i != 0 
				f << ",\n"
			f << "i32* getelementptr inbounds ([" << lines + 1 << "x i32],[" << lines + 1 << "x i32]* "
			f << "@" << key<< "line" << i << ", i32 0, i32 1)"
		}
		f << "]}\n"

		f << "@"<< key <<" = global " << machineType.GetName() << " {"
		f << "i32** bitcast (%" << key << "alltyp* @" << key << "all to i32**)"
		f << ",i32* bitcast (["<< states + 1 <<" x i32]* @" << key << "IsEndNode to i32*)"
		f << ",[256 x i8] ["
		for i : 256
		{
			if i != 0 f << ","
			f << "i8 " << chList[i]
		}
		f << "]}\n"
	}
}
