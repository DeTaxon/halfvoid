fopen := !(char^ Name,char^ s) -> void^ declare
fwrite := !(void^ data, int size, int count, void^ hndl) -> size_t declare
fread := !(void^ data, int size, int count, void^ hndl) -> size_t declare
fclose := !(void^ hndl) -> void declare
fgets := !(char^ buf,int Size,void^ Hnd) -> char^ declare
fputs := !(char^ buf,void^ Hnd) -> int declare
fprintf := !(void^ Hnd, char^ buf,...) -> int declare
feof := !(void^ hndl) -> int declare

TFile := class extend TIOStream
{
	handle := void^

	this := !(char^ a,char^ b) -> void
	{
		this.Open(a,b)
	}
	Open := !(char^ fileName, char^ flags) -> bool
	{
		handle = fopen(fileName,flags)
		return handle != null
	}
	Read := virtual !(void^ data, size_t size) -> size_t
	{
		return fread(data,1,size,handle) 
	}
	Write := virtual !(void^ data, size_t size) -> size_t
	{
		return fwrite(data,1,size,handle)
	}
	Close := virtual !() -> void
	{
		if handle == null 
			return void
		fclose(handle)
		handle = null
	}
}
