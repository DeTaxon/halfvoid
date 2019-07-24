Thread := class
{
	threadId := pthread_t
	this := !(void^ funcItem) -> void
	{
		pthread_create(threadId&,null,funcItem,null)
	}
	this := !(void^ funcItem, void^ arg) -> void
	{
		pthread_create(threadId&,nullptr,funcItem,arg)
	}
	Join := !() -> void
	{
		resVal := void^
		pthread_join(threadId,resVal&)
	}
}
