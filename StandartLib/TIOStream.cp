TIOStream := class
{
	Read := virtual !(void^ data, size_t size) -> size_t
	{
		return 0
	}
	Write := virtual !(void^ data, size_t size) -> size_t
	{
		return 0
	}
	Close := virtual !() -> void
	{
	}



	"<<" := !(char^ strData) self_return
	{
		Write(strData,strlen(strData))
		return this
	}
}
