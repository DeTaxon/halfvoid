fopen := !(char^ Name,char^ s) -> void^ declare
fclose := !(void^ hndl) -> void declare
fgets := !(char^ buf,int Size,void^ Hnd) -> char^ declare
feof := !(void^ hndl) -> int declare

file := class
{
	Handle := void^
	this := !(char^ a,char^ b) -> void
	{
		this.open(a,b)
	}
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
		return feof(Handle) == 0
	}
	IsOpen := !() -> bool
	{
		return Handle != null
	}
}

sfile := class extend file
{
	this := !(char^ a,char^ b) -> void
	{
		this.open(a,b)
	}
	readline := !() -> char^
	{
		Buff := char[4096]
		if fgets(Buff,4096,Handle) == null return ""
		return StrCopy(Buff)
	}	
}
