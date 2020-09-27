pthread_t := type u64
pthread_create := !(pthread_t^ thrPtr, void^ attrs, void^  callFunc,void^ arg) -> int declare
pthread_join := !(pthread_t thrId,void^^ retValue) -> int declare

CreateThread := !(void^ attrs,size_t stackSize,void^ startFunc,void^ arg,u32 flags,void^ threadId)-> void^ declare
WaitForSingleObject := !(void^ obj,u32 timeWait) -> void declare

Thread := class
{
	if $posix
		itData := pthread_t
	if $win32
		itData := void^
	this := !(!()&->void lambd) ->	void
	{
		proxyFunc := (void^ x) => 
		{
			_TaskPtr = null
			asLambd := x->{!()&->void}
			asLambd()
			asLambd.Destroy()
		}
		captLambd := lambd.Capture()->{void^}
		if $posix
			pthread_create(itData&,null,proxyFunc,captLambd)
		if $win32
			itData = CreateThread(null,0,proxyFunc,captLambd,0,null)
	}
	Join := !() -> void
	{
		if $posix
			pthread_join(itData,null)
		if $win32
		{
			infin := -1
			WaitForSingleObject(itData,infin&->{u32^}^)
			CloseHandle(itData)
		}
	}
}


