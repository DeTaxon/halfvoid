#import "string.cp"

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
DirectoryIterator := class
{
	toUse := Path^
	dirFd := void^
	nowEntry := void^
	nowPath := Path
	this := !(Path^ tu) -> void
	{
		toUse = tu
		dirFd = opendir(toUse.itStr)
		nowEntry =  readdir(dirFd)
		if GetDirectoryName(nowEntry) == "." nowEntry = readdir(dirFd)
		if GetDirectoryName(nowEntry) == ".." nowEntry = readdir(dirFd)
		if nowEntry != null
		{
			nowPath.itStr = toUse^.itStr + "/" + GetDirectoryName(nowEntry)
		}
	}
	"~this" := !() -> void
	{
		if dirFd != null closedir(dirFd)
	}
	"^" := !() -> ref Path
	{
		return nowPath
	}
	Inc := !() -> void
	{
		nowEntry = readdir(dirFd)
		if nowEntry != null
		{
			newName := GetDirectoryName(nowEntry)
			nowPath.itStr = toUse.itStr + "/" + GetDirectoryName(nowEntry)
		}
	}
	IsEnd := !() -> bool
	{
		return nowEntry == null
	}
}


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
		defer free(pathA)

		pathB := realpath(pt2.itStr,null->{char^})
		if pathB == null return false
		defer free(pathB)

		return pathA == pathB
	}
	"~For" := !() -> DirectoryIterator
	{
		return DirectoryIterator(this&)
	}
	IsDir := !() -> bool
	{
		statStruct := char[144]
		res := stat(itStr,statStruct->{char^})
		return (statStruct[25] and_b 0x40) != 0
	}
	"/=" := !(string str) -> void
	{
		itStr = itStr + "/" + str
	}
	"/" := !(string pt2) -> Path
	{
		ToRet.itStr = this.itStr + "/" + pt2
	}
	"/" := !(Path pt2) -> Path
	{
		return Path(itStr + "/" + pt2.itStr)
	}
}
