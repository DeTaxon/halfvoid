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
