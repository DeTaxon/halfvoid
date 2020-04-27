Files := Queue.{BoxFile^}
ZipFiles := Queue.{BoxFile^}

LoadFile := !(string name,Object^ start) -> BoxFile^
{
	if start == null return null
	iter := start
	while iter.Up != null iter = iter.Up

	asF := iter->{BoxFile^}
	prePath := Path(asF.filePath.itStr)
	prePath = prePath.FolderName()
	prePath /= name
	newName := prePath.FullPath()
	newPath := Path(newName)

	return LoadFile(newPath)
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
	ob := GetObjectsFromFile2(fullName)

	if ob == null 
	{
		ErrorLog.Push("file "+ fullName.itStr +" not found\n")
		return null
	}

	Files.Push(ob->{BoxFile^})
	return ob->{BoxFile^}
}
LoadZipFile := !(Path fullName,Queue.{void^} res,List.{char^} suf) -> void
{
	newZip := new ZipFile ; $temp
	newZip.AnalizeFile(fullName.itStr)
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
	newZip.DecUser()

}

ImportCmd := class extend Object
{
	toImport := string
	toPoint := BoxFile^
	this := !(string toAdd) ->void
	{
		toImport = toAdd
		WorkBag.Push(this&,State_Load)
	}
	GetValue := virtual !() -> string
	{
		return "#import cp"
	}
	GetFile := !() -> BoxFile^
	{
		if toPoint == null
		{
			toPoint = LoadFile(toImport,this&)

			if toPoint == null
			{
				ErrorLog.Push("file " + toImport + " not found\n")
			}
		}
		return toPoint
	}
	DoTheWork := virtual !(int pri) -> void
	{
		if toPoint == null
		{
			toPoint = LoadFile(toImport,this&)
			if toPoint != null
			{
				WorkBag.Push(toPoint,State_Start)
			}
		}
		if Up != and Up is BoxFile{
			asNeed := Up->{BoxFile^}
			asNeed.ImportingFiles.Push(toPoint)
		}
	}
}


