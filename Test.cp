printf := !(char^ str, ...) -> int declare
calloc := !(int size,int sizet) -> void^ declare
malloc := !(int size) -> void^ declare
free := !(void^ point) -> void declare

dlopen := !(char^ name,int pri) -> void^ declare
dlsym := !(void^ hndl, char^ name) -> void^ declare
dlclose := !(void^ hndl) -> int declare


//vkEnumerateInstanceExtensionProperties := !(

VkLayerProperties := class
{
	name := char[256]
	version := s32
	iVer := s32
	descr := char[256]
}
vkEnumerateInstanceLayerProperties := !(int^ , int^)^ -> void


ve := class
{
	x,y,z := float
	this := !() -> void
	{
	}
	print := !(int u) -> void
	{
		printf("hello method %f %f %f %i\n",x,y,z,u)
	}
	print2 := !() -> void
	{
		print(15)
	}
}

ve2 := class extend ve
{
	w := int
	this := !() -> void
	{
		w = 0
	}
}
main := !(int argc, char^^ argv) -> int
{
	handl := dlopen("libvulkan.so.1",2)
	count := s32
	vkEnumerateInstanceLayerProperties = dlsym(handl,"vkEnumerateInstanceLayerProperties")


	printf("lol %p\n",vkEnumerateInstanceLayerProperties )
	dlclose(handl)
	return 0
}
