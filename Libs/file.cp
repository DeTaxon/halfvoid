fopen := !(char^ Name,char^ s) -> void^ declare
fwrite := !(void^ data, int size, int count, void^ hndl) -> int declare
fclose := !(void^ hndl) -> void declare
fgets := !(char^ buf,int Size,void^ Hnd) -> char^ declare
fputs := !(char^ buf,void^ Hnd) -> int declare
fprintf := !(void^ Hnd, char^ buf,...) -> int declare
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
	write := !(void^ data, int Size) -> int
	{	
		return fwrite(data,Size,1,Handle)
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
	"<<" := !(string likeCpp) -> ref sfile
	{
		fputs(likeCpp,Handle)
		return this
	}
	"<<" := !(s64 x) -> ref sfile
	{
		fprintf(Handle,"%li",x)
		return this
	}
	"<<" := !(int x) -> ref sfile
	{
		fprintf(Handle,"%i",x)
		return this
	}
	"<<" := !(float x) -> ref sfile
	{
		fprintf(Handle,"%f",x)
		return this
	}
	"<<" := !(double x) -> ref sfile
	{
		fprintf(Handle,"%f",x)
		return this
	}
}
