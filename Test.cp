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

lol := !(x) -> void
{
	printf("world\n")
}
lol := !(this) -> void
{
	printf("hello\n")
}
max := !(x,y) -> int
{
	if x > y return x
	return y
}

main := !(int argc, char^^ argv) -> int
{
	3.lol()
	lol(3)
	
	a := int
	a = max(1,2)
	x := 12

	for 3..x printf("wow %i\n",it)

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





