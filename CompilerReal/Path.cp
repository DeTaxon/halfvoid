//#import "string.cp"

realpath := !(char^ inp, char^ outp) -> char^ declare

opendir := !(char^ inp) -> void^ declare
closedir := !(void^ dirFd) ->int declare
readdir := !(void^ dirFd) -> void^ declare
stat := !(char^ pth, void^ outStat) ->int declare

GetDirectoryName := !(void^ dirFd) -> char^
{
	itAsChar := dirFd->{char^}
	return itAsChar[19]&
}
//st_mode 24 size = 144
//DirectoryIterator := class
//{
//	toUse := Path^
//	dirFd := void^
//	nowEntry := void^
//	nowPath := Path
//	this := !(Path^ tu) -> void
//	{
//		toUse = tu
//		dirFd = opendir(toUse.itStr)
//		nowEntry =  readdir(dirFd)
//		if GetDirectoryName(nowEntry) == "." nowEntry = readdir(dirFd)
//		if GetDirectoryName(nowEntry) == ".." nowEntry = readdir(dirFd)
//		if nowEntry != null
//		{
//			nowPath.itStr = toUse^.itStr + "/" + GetDirectoryName(nowEntry)
//		}
//	}
//	"~this" := !() -> void
//	{
//		if dirFd != null closedir(dirFd)
//	}
//	"^" := !() -> ref Path
//	{
//		return nowPath
//	}
//	Inc := !() -> void
//	{
//		nowEntry = readdir(dirFd)
//		if nowEntry != null
//		{
//			newName := GetDirectoryName(nowEntry)
//			nowPath.itStr = toUse.itStr + "/" + GetDirectoryName(nowEntry)
//		}
//	}
//	IsEnd := !() -> bool
//	{
//		return nowEntry == null
//	}
//}


Path := class
{
	itStr := string
	this := !() -> void
	{
		itStr = null
	}
	this := !(string st) -> void
	{
		itStr = st
	}
	"==" := !(Path pt2) -> bool
	{
		pathA := realpath(itStr,null->{char^})
		if pathA == null return false
		//defer free(pathA)

		pathB := realpath(pt2.itStr,null->{char^})
		if pathB == null return false
		//defer free(pathB)

		return pathA == pathB
	}
	IsExist := !() -> bool
	{
		newLine := realpath(itStr,null->{char^})
		if newLine == null return false
		free(newLine)
		return true
	}
	FullPath := !() -> string
	{
		newLine := realpath(itStr,null->{char^})
		if newLine == null return ""
		return newLine
	}
	//"~For" := !() -> DirectoryIterator
	//{
	//	return DirectoryIterator(this&)
	//}
	//IsDir := !() -> bool
	//{
	//	statStruct := char[144]
	//	res := stat(itStr,statStruct->{char^})
	//	return (statStruct[25] and_b 0x40) != 0
	//}
	"=" := !(string inp) -> void
	{
		itStr = inp
	}
	"=" := !(Path inp) -> void
	{
		itStr = inp.itStr
	}
	"/=" := !(string str) -> void
	{
		itStr = itStr + "/" + str
	}
	//"/" := !(string pt2) -> Path
	//{
	//	ToRet.itStr = this.itStr + "/" + pt2
	//}
	//"/" := !(Path pt2) -> Path
	//{
	//	return Path(itStr + "/" + pt2.itStr)
	//}
	//Ext := !() -> string
	//{
	//	size := StrSize(itStr)
	//	pos := size - 1
	//	if size == 0 return ""

	//	while pos >= 0 and itStr[pos] != '.' {
	//		if itStr[pos] in "/\\"  return ""
	//		pos--
	//	}
	//	if pos < 0 return ""

	//	return StrCopy(itStr[pos]&)
	//}
	//Name := !() -> string
	//{
	//	size := StrSize(itStr)
	//	pos := size - 1
	//	if size == 0 return ""

	//	while pos >= 0 {
	//		if itStr[pos] in "/\\"  break
	//		pos--
	//	}
	//	if pos < 0 return ""
	//	if size - pos == 1 return ""

	//	return StrCopy(itStr[pos + 1]&)
	//}
	PathName := !() -> string
	{
		size := StrSize(itStr)
		pos := size - 1
		if size == 0 return "."
		
		keepS := true
		while pos >= 0 and keepS{
			if itStr[pos] in "/\\"  keepS = false
			if keepS
			{
				pos -= 1
			}
		}
		if pos < 0 return "."
		if size - pos == 1 return "."
		pos += 1
		newStr :=  malloc(pos + 1)->{char^}

		for i : pos newStr[i] = itStr[i]
		newStr[pos] = 0

		return newStr
	}
	//JustName := !() -> string
	//{
	//	itName := this.Name()
	//	if itName == "" return ""
	//	defer delete itName
	//	
	//	itSize := StrSize(itName)
	//	pos := itSize - 1

	//	while pos >= 0 and itName[pos] != '.' pos--

	//	if pos < 0 return itName
	//	itNewName := malloc(pos + 1)->{char^}
	//	for i : pos itNewName[i] = itName[i]
	//	itNewName[pos] = 0
	//	return itNewName
	//}
}
