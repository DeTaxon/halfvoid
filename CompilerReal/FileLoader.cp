
Files := Queue.{BoxFile^}

LoadFile := !(string name,Object^ start) -> BoxFile^
{
	if start == null return null
	iter := start
	while iter.Up != null iter = iter.Up

	asF := iter->{BoxFile^}
	prePath := Path(asF.filePath.itStr)
	prePath = prePath.PathName()
	prePath /= name
	newName := prePath.FullPath()
	newPath := Path(newName)
	return LoadFile(newPath)
}
LoadFile := !(Path fullName) -> BoxFile^
{
	for Files.Size()
	{
		if Files[it].filePath == fullName
			return Files[it]
	}
	printf("loading %s\n",fullName.itStr)
	ob := GetObjectsFromFile(fullName)

	if ob == null 
	{
		ErrorLog.Push("file "+ fullName.itStr +" not found\n")
		return null
	}

	Files.Push(ob->{BoxFile^})
	return ob->{BoxFile^}
}

ImportCmd := class extend Object
{
	toImport := string
	toPoint := BoxFile^
	this := !(string toAdd) ->void
	{
		toImport = toAdd
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
	}
}


