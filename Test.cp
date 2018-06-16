#import "lib.cp"
//#import "main.cp"


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
//close := !(int fd) -> void declare

mmap := !(void^ addt,s64 len,int prot, int flags, int fd, void^ offset) -> void^ declare

//ArrayIterMappedFile := class
//{	
//	x := int
//}
MappedFile := class
{
	name := string
	itemId := int
	size  := s64
	point := char^
	this := !(char^ fileName) -> void
	{
		itemId = open(fileName,2)
		printf("name %i\n",itemId)

		//if true
		//{
			//if 
				val := GetFileSizeLinux(itemId,size&)
				printf("size %i %i\n",val,size)
	
			//{
			//	if size == 0{
			//		itemId = 0 - 1
			//	}else{
					point = mmap(null,size,PROT_NONE,MAP_SHARED,itemId,null)
					printf("point %p\n",point)

			//		if point == null {
			//			itemId = 0 - 1						
			//		}else{
			//			//good?
			//		}
			//	}
			//}else{
			//	itemId =  0 - 1
			//}
		//}
	}
	IsInvalid := !() -> bool
	{
		return itemId
	}

	Size := !() -> int
	{
		return size
	}
	Close := !() -> void
	{
		
	}
}


main := !(int argc, char^^ argv) -> int
{
	c := MappedFile("Test.cp")
	//if c.itemId == 0 - 1
	//{
	//	//printf("invalid file\n")
	//}else{
		printf("part %i\n",c.size)
	//}
	return 0
}

