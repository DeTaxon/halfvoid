printf := !(char^ str, ...) -> int declare
calloc := !(int size,int sizet) -> void^ declare
malloc := !(int size) -> void^ declare
free := !(void^ point) -> void declare

dlopen := !(char^ name,int pri) -> void^ declare
dlsym := !(void^ hndl, char^ name) -> void^ declare
dlclose := !(void^ hndl) -> int declare


////vkEnumerateInstanceExtensionProperties := !(
//
//VkLayerProperties := class
//{
//	name := char[256]
//	version := s32
//	iVer := s32
//	descr := char[256]
//}
//vkEnumerateInstanceLayerProperties := !(int^ , VkLayerProperties^)^ -> void
//main := !(int argc, char^^ argv) -> int
//{
//	handl := dlopen("libvulkan.so.1",2)
//	count := s32
//	nums := VkLayerProperties^
//	vkEnumerateInstanceLayerProperties = dlsym(handl,"vkEnumerateInstanceLayerProperties")
//
//	vkEnumerateInstanceLayerProperties(count&,null)
//
//	nums = new VkLayerProperties[8]
//
//	vkEnumerateInstanceLayerProperties(count&,nums)
//
//	i := 0
//	while i < count
//	{
//		printf("%i : %s\n",i,nums[i].name)
//		i += 1
//	}
//
//	dlclose(handl)
//	return 0
//}

Arr := class !{a}
{
	ar := a[13]
}

main := !(int argc,char^^ argv) -> int
{
	c := Arr.{char}
	h := Arr.{int}
	return 0
}



