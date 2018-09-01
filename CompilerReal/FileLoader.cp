
Files := Queue.{BoxFile^}

LoadFile := !(string name) -> BoxFile^
{
	return LoadFile(name,null->{Object^})
}
LoadFile := !(string name, Object^ start) -> BoxFile^
{
	//TODO: check for 
	// import "file" vs "folder/file"
	for Files.Size()
	{
		if Files[it].fileName == name
			return Files[it]
	}
	ob := GetObjectsFromFile(name)

	if ob == null 
	{
		ErrorLog.Push("file "+ name +" not found\n")
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
			toPoint = LoadFile(toImport)

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
			toPoint = LoadFile(toImport)
			if toPoint != null
			{
				WorkBag.Push(toPoint,State_Start)
			}
		}
	}
}


