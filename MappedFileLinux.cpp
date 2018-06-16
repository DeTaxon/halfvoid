#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern "C"{
	bool GetFileSizeLinux(int fd,__int64_t* size)
	{
		struct stat sb;
		if (fstat(fd,&sb) == -1) return false;
		*size = sb.st_size;
		return true;
	}
}

//int main()
//{
//	printf("flage %i %i %i\n",PROT_READ,PROT_WRITE,PROT_NONE);
//	printf("flage %i %i %i\n",MAP_SHARED,MAP_PRIVATE,MAP_ANONYMOUS);
//	return 0;
//}
