#import "lib.cp"

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
open := !(char^ name,int flags) -> int declare
close := !(int fd) -> void declare

truncate := !(int fd,s64 size) -> int declare

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
	size  := int
	point := char^
	this := !(char^ fileName) -> void
	{
		itemId = open(fileName,O_RDWR)

		if itemId == -1 return void

		bigSize := s64
		val := GetFileSizeLinux(itemId,bigSize&)
		size = bigSize->{int}

		if not val
		{
			close(itemId)
			itemId = -1
			return void
		}

		if size == 0 return void
	
		point = mmap(null,bigSize,PROT_READ + PROT_WRITE,MAP_SHARED,itemId,null)
		if point == null
		{
			close(itemId)
			itemId = -1
		}
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
}
