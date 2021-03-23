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
	"<<" := !(int value) self_return
	{
		intVal := char[16]
		len := sprintf(intVal[0]&,"%i",value)
		Write(intVal,len)
		return this
	}
	"<<" := !(s64 value) self_return
	{
		intVal := char[40]
		len := sprintf(intVal[0]&,"%li",value)
		Write(intVal,len)
		return this
	}
	"<<" := !(double value) self_return
	{
		intVal := char[80]
		len := snprintf(intVal[0]&,80,"%f",value)
		Write(intVal,len)
		return this
	}
}
