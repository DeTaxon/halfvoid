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

ve := class
{
	x := int
	print := virtual !() -> void
	{
		printf("nope\n")
	}
}
ve2 := class extend ve
{
	print := virtual !() -> void
	{
		printf("yea\n")
	}
}

v := 3.5

main := !(int argc, char^^ argv) -> int
{
	v := new ve2
	vb := ve^
	vb = v&
	vb.x = 2
	vb.print()
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





