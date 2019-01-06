
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

sem_t := class
{
	sAlign := s64
	sFields := char[24]
}

pthread_t := type u64

//sem_open := !(char^ name, int oflags) -> sem_t^ declare
sem_init := !(sem_t^ semPtr,int pshared, u32 value) -> int declare
sem_wait := !(sem_t^ semPtr) -> int declare
sem_post := !(sem_t^ semPtr) -> int declare
sem_destroy := !(sem_t^ semPtr) -> int declare

pthread_create := !(pthread_t^ thrPtr, void^ attrs, void^  callFunc,void^ arg) -> int declare
pthread_join := !(pthread_t thrId,void^^ retValue) -> int declare

open := !(char^ name,int flags,int mode) -> int declare
close := !(int fd) -> void declare

sleep := !(int seconds) -> int declare

ftruncate := !(int fd,s64 size) -> int declare

mmap := !(void^ addt,s64 len,int prot, int flags, int fd, void^ offset) -> void^ declare
munmap := !(void^ addt,s64 len) -> int declare

fstat := !(int fd,char^ st) -> int declare
