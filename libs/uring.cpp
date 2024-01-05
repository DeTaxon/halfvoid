#ifdef __linux__

#include <liburing.h>
#include <stdio.h>
#include <stdlib.h>
extern "C"
{

size_t ur_ring_size()
{
	return sizeof(io_uring);
}

void* ur_create()
{
	return malloc(sizeof(io_uring));
}
void ur_init(int entrys,io_uring* ring,int flags)
{
	io_uring_queue_init(entrys,ring,flags);
}
void ur_exit(io_uring* ring)
{
	//io_uring_queue_exit(ring);
}
void ur_destroy(void* ptr)
{
	free(ptr);
}

void* ur_get_sqe(io_uring* ring)
{
	return io_uring_get_sqe(ring);
}

void ur_sqe_set_data(io_uring_sqe* sqe, void* data)
{
	io_uring_sqe_set_data(sqe,data);
}
void* ur_cqe_get_data(io_uring_cqe* cqe)
{
	return io_uring_cqe_get_data(cqe);
}

void ur_sqe_poll_add(io_uring_sqe* sqe, int fd, int poll_mask) 
{
	io_uring_prep_poll_add(sqe,fd,poll_mask);
}

void ur_sqe_timeout(io_uring_sqe* sqe, double timeout, int count, int flags,void* extra)
{
	struct __kernel_timespec* ts = (__kernel_timespec*)extra;
	ts->tv_sec = (__s64)timeout;
	ts->tv_nsec = timeout*1000*1000*1000;
	ts->tv_nsec = ts->tv_nsec % (1000*1000*1000);
	io_uring_prep_timeout(sqe,ts,count,flags);
}

void ur_sqe_readv(io_uring_sqe* sqe,int fd,iovec* vecs, int count,size_t offset)
{
	io_uring_prep_readv(sqe,fd,vecs,count,offset);
}
void ur_sqe_writev(io_uring_sqe* sqe,int fd,iovec* vecs, int count,size_t offset)
{
	io_uring_prep_writev(sqe,fd,vecs,count,offset);
}

int ur_wait_cqe_timeout(io_uring* ring,io_uring_cqe** cqe_ptr, double timeout)
{
	struct __kernel_timespec ts;
	ts.tv_sec = (__s64)timeout;
	ts.tv_nsec = timeout*1000*1000*1000;
	ts.tv_nsec = ts.tv_nsec % (1000*1000*1000);
	return io_uring_wait_cqe_timeout(ring,cqe_ptr,&ts);
}

void ur_submit(io_uring* ring)
{
	io_uring_submit(ring);
}

void ur_cqe_seen(io_uring* ring,io_uring_cqe* cqe)
{
	io_uring_cqe_seen(ring,cqe);	
}

}
#endif
