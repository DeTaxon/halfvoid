#import "PosixFuncs.cp"

Mutex := class 
{
	semItem := sem_t
	this := !() -> void
	{
		sem_init(semItem&,0,1)
	}
	this := !(int depth) -> void
	{
		sem_init(semItem&,0,depth)
	}
	Lock := !() -> void
	{
		sem_wait(semItem&)
	}
	Unlock := !() -> void
	{
		sem_post(semItem&)
	}
	Close := !() -> void
	{
		sem_destroy(semItem&)
	}
}
