#import "lib.cp"
#import "PosixFuncs.cp"

FILE_READ := 1
FILE_WRITE := 2
FILE_CREATE := 3



GetFileSizeLinux2 := !(int fd,s64^ size) -> bool
{
	dataD := char[144]
	val := fstat(fd,dataD->{char^})
	if val == -1 return false
	asNeed1 := dataD->{char^}[48]&
	asNeed2 := asNeed1->{s64^}
	size^ = asNeed2^
	return true
}
ArrayIterMappedFile := class
{	
	x := s64
	pFile := void^
	this := !(void^ pF) -> void
	{
		x = 0
		pFile = pF
	}
	"^" := !() -> ref u8
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
	point := u8^
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
		if flg == FILE_CREATE fdFlag = O_CREAT + O_RDWR

		itemId = open(fileName,fdFlag,511)

		if itemId == -1 return void

		val := GetFileSizeLinux2(itemId,size&)

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
	"[]" := !(int pos) -> ref u8
	{	
		return point[pos]
	}
	Close := !() -> void
	{
		if itemId == -1 return void
		close(itemId)
		if point == null return void
		munmap(point,size)
		itemId = -1
	}

	makeMap := !() -> void
	{
		PROTS := PROT_READ
		SHARES := MAP_PRIVATE
		if flag != FILE_READ 
		{
			PROTS = PROTS or_b PROT_WRITE
			SHARES = MAP_SHARED
		}
		SHARES += 32768
		point = mmap(null,size,PROTS,SHARES,itemId,null)
		if point == null //TODO: in error it returns 0xFFFFFFFFFFFFF
		{
			close(itemId)
			itemId = -1
		}
	}
}
