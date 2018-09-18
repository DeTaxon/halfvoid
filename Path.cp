#import "string.cp"

realpath := !(char^ inp, char^ outp) -> char^ declare

opendir := !(char^ inp) -> void^ declare
readdir := !(void^ dirFd) -> void^ declare
stat := !(char^ pth, void^ outStat) ->int declare

GetDirectoryName := !(void^ dirFd) -> char^
{
	itAsChar := dirFd->{char^}
	return itAsChar + 19
}
//st_mode 24 size = 144
DirectoryIterator := class
{
	toUse := Path^
	dirFd := void^
	nowEntry := void^
	nowPath := Path
	this := !(void^ tu) -> void
	{
		toUse = tu->{Path^}
		dirFd = opendir(toUse.itStr)
		nowEntry =  readdir(dirFd)
		if GetDirectoryName(nowEntry) == "." nowEntry = readdir(dirFd)
		if GetDirectoryName(nowEntry) == ".." nowEntry = readdir(dirFd)
		if nowEntry != null
		{
			nowPath.itStr = toUse^.itStr + "/" + GetDirectoryName(dirFd)
		}
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
			nowPath.itStr = toUse.itStr + "/" + GetDirectoryName(dirFd)
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
		printf("wut %o %o %o %o\n",statStruct[24],statStruct[25],statStruct[26],statStruct[27])
		return (statStruct[25] and_b 4) != 0
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
