
c := thread_local int
d := thread_local bool

testWow := !() -> void
{
	c = 7
	d = false
}

main := !(int argc, char^^ argv) -> int
{
	testWow()
	printf("hah %i %i\n",c,d)
	//handl := dlopen("libvulkan.so.1",2)
	//count := s32

	//getVKAddr := PFN_vkGetDeviceProcAddr
	//vkEnumerateInstanceLayerProperties := PFN_vkEnumerateInstanceLayerProperties
	//cInst := PFN_vkCreateInstance


	//getVKAddr = null
	//getVKAddr = dlsym(handl,"vkGetDeviceProcAddr")

	//if getVKAddr == null{
	//	printf("cant get function ProcAddr\n")
	//	return 0
	//}

	//printf("a\n")
	//cInst := getVKAddr(null,"vkCreateInstance")
	//printf("a\n")

	//if cInst == null {
	//	printf("can not get function get instance\n")
	//	return 0
	//}

	//newFunc := getVKAddr(null,"vkEnumerateInstanceLayerProperties")
	//printf("a\n")
	//vkEnumerateInstanceLayerProperties = newFunc->{PFN_vkEnumerateInstanceLayerProperties}

	//if newFunc == null{
	//	printf("cant get func props\n")
	//	return 0
	//}

	//vkEnumerateInstanceLayerProperties(count&,null)

	//nums := new VkLayerProperties[count] ; $temp

	//vkEnumerateInstanceLayerProperties(count&,nums)

	//for i : count printf("%i : %s\n",i,nums[i].layerName)

	//dlclose(handl)
	return 0
}

