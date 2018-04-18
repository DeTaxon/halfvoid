#import "lib.cp"

//vkEnumerateInstanceExtensionProperties := !(

VkLayerProperties := class
{
	name := char[256]
	version := s32
	iVer := s32
	descr := char[256]
}
vkEnumerateInstanceLayerProperties := !(int^ , VkLayerProperties^)^ -> void


v := 3.5

"in" := !(int x, int y) -> int
{
	printf("test\n")
	return 0
}

main := !(int argc, char^^ argv) -> int
{
	2 in 2
	return 0
	handl := dlopen("libvulkan.so.1",2)
	count := s32

	vkEnumerateInstanceLayerProperties = dlsym(handl,"vkEnumerateInstanceLayerProperties")

	vkEnumerateInstanceLayerProperties(count&,null)

	nums := new VkLayerProperties[count]

	vkEnumerateInstanceLayerProperties(count&,nums)

	for i : count printf("%i : %s\n",i,nums[i].name)

	dlclose(handl)
	return 0
}





