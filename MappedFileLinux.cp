#import "lib.cp"

FILE_READ := 1
FILE_WRITE := 2
FILE_CREATE := 3

O_RDONLY := 0
O_WRONLY := 1
O_RDWR := 2

O_APPEND := 1024
O_CREAT := 64
O_TRUNC := 512
O_TMPFILE := 4259840

PROT_READ := 1
PROT_WRITE := 2
PROT_NONE := 0

MAP_SHARED := 1
MAP_PRIVATE := 2
MAP_ANON := 32 

GetFileSizeLinux := !(int fd,s64^ size) -> bool declare

open := !(char^ name,int flags,int mode) -> int declare
close := !(int fd) -> void declare

ftruncate := !(int fd,s64 size) -> int declare

mmap := !(void^ addt,s64 len,int prot, int flags, int fd, void^ offset) -> void^ declare
munmap := !(void^ addt,s64 len) -> int declare

ArrayIterMappedFile := class
{	
	x := int
	pFile := void^
	this := !(void^ pF) -> void
	{
		x = 0
		pFile = pF
	}
	"^" := !() -> ref char
	{
		pP := pFile->{MappedFile^}
		return pP.point[x]
	}
	Inc := !() -> void
	{
		x += 1
	}
	IsEnd := !() -> bool
	{
		pP := pFile->{MappedFile^}
		return x >= pP.size
	}
	IsInvalid := !() -> bool
	{
		pP := pFile->{MappedFile^}
		return pP.IsInvalid()
	}
}
MappedFile := class
{
	name := string
	itemId := int
	flag := int
	size  := s64
	point := char^
	this := !(char^ fileName) -> void
	{
		itemId = -1
		point = null
		Open(fileName,FILE_READ,0)
	}
	this := !(char^ fileName,int flg) -> void
	{
		itemId = -1
		point = null
		if flg == FILE_CREATE
			Open(fileName,FILE_WRITE,0)
		else
			Open(fileName,flg,0)
	}
	this := !(char^ fileName,int flg, s64 itSize) -> void
	{
		itemId = -1
		point = null
		Open(fileName,flg,itSize)
	}
	Open := !(char^ fileName,int flg, s64 itSize) -> void
	{
		flag = flg
		fdFlag := int
		if flg == FILE_READ
			fdFlag = O_RDONLY
		else
			fdFlag = O_RDWR
		if flg == FILE_CREATE fdFlag += O_CREAT

		itemId = open(fileName,fdFlag,511)

		if itemId == -1 return void

		val := GetFileSizeLinux(itemId,size&)

		if not val
		{
			close(itemId)
			itemId = -1
			return void
		}

		if flg == FILE_CREATE
		{
			if itSize != size Resize(itSize)
				else makeMap()
		}else{
			if size == 0 return void
			makeMap()
		}
	}
	Resize := !(s64 newSize) -> void
	{
		if IsInvalid() return void
		if newSize < 0 return void

		if point != null munmap(point,size)
		w := ftruncate(itemId,newSize)
		size = newSize
		makeMap()
	}
	IsInvalid := !() -> bool
	{
		if itemId == -1 return true
		return false
	}

	Size := !() -> int
	{
		return size
	}
	"~For" := !() -> ArrayIterMappedFile
	{
		return ArrayIterMappedFile(this&)
	}
	"[]" := !(int pos) -> ref char
	{	
		return point[pos]
	}
	Close := !() -> void
	{
		if itemId == -1 return void
		close(itemId)
		if point == null return void
		munmap(point,size)
	}

	makeMap := !() -> void
	{
		PROTS := PROT_READ
		SHARES := MAP_PRIVATE
		if flag != FILE_READ 
		{
			PROTS += PROT_WRITE
			SHARES = MAP_SHARED
		}

		point = mmap(null,size,PROTS,SHARES,itemId,null)
		if point == null //TODO: in error it returns 0xFFFFFFFFFFFFF
		{
			close(itemId)
			itemId = -1
		}
	}
}
