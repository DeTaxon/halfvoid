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


v := 15

main := !(int argc, char^^ argv) -> int
{
	handl := dlopen("libvulkan.so.1",2)
	count := s32
	nums := VkLayerProperties^
	vkEnumerateInstanceLayerProperties = dlsym(handl,"vkEnumerateInstanceLayerProperties")

	vkEnumerateInstanceLayerProperties(count&,null)

	nums = new VkLayerProperties[count]

	vkEnumerateInstanceLayerProperties(count&,nums)

	for i : count printf("%i : %s\n",i,nums[i].name)

	dlclose(handl)
	return 0
}





