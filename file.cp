fopen := !(char^ Name,char^ s) -> void^ declare
fclose := !(void^ hndl) -> void declare
fgets := !(char^ buf,int Size,void^ Hnd) -> char^ declare
feof := !(void^ hndl) -> int declare

file := class
{
	Handle := void^
	open := !(char^ Name,char^ flags) -> bool
	{
		Handle = fopen(Name,flags)
		return Handle != null
	}
	close := !() -> void
	{
		fclose(Handle)
	}
	read := !(void^ data, int Size) -> int 
	{
		return 0	
	}
	good := !() -> bool
	{
		return feof(Handle) != 0
	}
	IsOpen := !() -> bool
	{
		return Handle != null
	}
}

sfile := class extend file
{
	readline := !() -> char^
	{
		Buff := char[4096]
		if fgets(Buff,4096,Handle) == null return null
		return StrCopy(Buff)
	}	
}
