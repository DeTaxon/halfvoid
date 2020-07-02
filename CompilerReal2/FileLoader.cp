Files := Queue.{BoxFile^}
ZipFiles := Queue.{BoxFile^}

LoadFile := !(string name,Object^ start) -> BoxFile^
{
	if start == null return null
	iter := start
	while iter.Up != null iter = iter.Up

	asF := iter->{BoxFile^}
	prePath := ""sbt + asF.filePath.itStr
	prePath2 := ""sbt + Path(prePath).FolderName()
	prePath2 << "/" << name
	newPath := Path(prePath2).FullPath()

	return LoadFile(Path(StrCopy(newPath.Get())))
}
LoadFile := !(Path fullName) -> BoxFile^
{
	for Files
	{
		if it.filePath == fullName
			return it
	}
	//printf("loading %s\n",fullName.itStr)
	//#critical_atomic
	ob := GetObjectsFromFile2(fullName)->{BoxFile^}

	if ob == null 
	{
		ErrorLog.Push("file "sbt + fullName.itStr +" not found\n")
		return null
	}

	Files.Push(ob)
	ob.cs = InfiniteCodeSpace
	return ob
}
LoadZipFile := !(Path fullName,Queue.{void^} res,List.{char^} suf) -> void
{
	newZip := new ZipFile ; $temp
	newZip.AnalizeFile(fullName.itStr)
	defer newZip.DecUser()
	for fil : newZip^
	{
		if fil.realSize == 0
			continue
		shouldUse := false
		for s : suf
		{
			sLen := StrSize(s)
			if fil.fullName.Size() >= sLen and fil.fullName[-sLen..0] == s
			{
				shouldUse = true
				break
			}
		}
		if not shouldUse
			continue
			
		itPtr := fil.Map()
		itNamePre := ""sbt << fullName.itStr << "/" << fil.fullName.Str()
		itName := itNamePre.Str()
		ob := GetObjectsFromMemory(Path(itName),itPtr->{char^},fil.realSize)
		res.Push(ob)
		ZipFiles.Push(ob)
		fil.Unmap()
	}

}


