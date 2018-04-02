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
vkEnumerateInstanceLayerProperties := !(int^ , VkLayerProperties^)^ -> void
main := !(int argc, char^^ argv) -> int
{
	handl := dlopen("libvulkan.so.1",2)
	count := s32
	nums := VkLayerProperties^
	vkEnumerateInstanceLayerProperties = dlsym(handl,"vkEnumerateInstanceLayerProperties")

	vkEnumerateInstanceLayerProperties(count&,null)

	size := int
	size = 0
	size = 256 + 256 + 8
	size *= count
	nums = malloc(size)

	vkEnumerateInstanceLayerProperties(count&,nums)

	i := int
	i = 0
	while i < count
	{
		printf("%i : %s\n",i,nums[i]&)
		i -= 1
	}


	printf("lol %p\n",vkEnumerateInstanceLayerProperties )
	dlclose(handl)
	return 0
}
