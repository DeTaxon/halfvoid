
vkLoadAddr := PFN_vkGetInstanceProcAddr
vkEnumerateInstanceExtensionProperties := PFN_vkEnumerateInstanceExtensionProperties
vkEnumerateInstanceLayerProperties := PFN_vkEnumerateInstanceLayerProperties
vkCreateInstance := PFN_vkCreateInstance



testSt := Stack.{int}

huh := !() -> void
{
	printf("----\n")
	printf("1: %i\n",testSt.Start?.Data)
	printf("2: %i\n",testSt.Start?.Next?.Data)
	printf("3: %i\n",testSt.Start?.Next?.Next?.Data)

	if testSt.Start?.Next?.Data == 16
	{
		printf("hallo\n")
	}
}


main := !(int argc, char^^ argv) -> int
{
	huh()
	testSt.Push(234)
	huh()
	testSt.Push(456)
	huh()
	testSt.Push(16)
	huh()



	//handl := dlopen("libvulkan.so.1",2)

	//vkLoadAddr = dlsym(handl,"vkGetInstanceProcAddr")

	//if vkLoadAddr == null{
	//	printf("cant get function ProcAddr\n")
	//	return 0
	//}

	//newFunc := void^
	//newFunc = vkLoadAddr(null,"vkEnumerateInstanceExtensionProperties")
	//if newFunc == null{ printf("cant get func props\n")	return 0	}
	//vkEnumerateInstanceExtensionProperties = newFunc->{PFN_vkEnumerateInstanceExtensionProperties}

	//newFunc = vkLoadAddr(null,"vkEnumerateInstanceLayerProperties")
	//if newFunc == null{ printf("cant get func props\n")	return 0	}
	//vkEnumerateInstanceLayerProperties = newFunc->{PFN_vkEnumerateInstanceLayerProperties}

	//newFunc = vkLoadAddr(null,"vkCreateInstance")
	//if newFunc == null{ printf("cant get func props\n")	return 0	}
	//vkCreateInstance = newFunc->{PFN_vkCreateInstance}

	//count := s32

	//vkEnumerateInstanceLayerProperties(count&,null)
	//lays := new VkLayerProperties[count] ; $temp
	//vkEnumerateInstanceLayerProperties(count&,lays)

	//printf("%i : %s\n",i,lays[^i].layerName)

	//vkEnumerateInstanceExtensionProperties(null,count&,null)
	//exts := new VkExtensionProperties[count] ; $temp
	//vkEnumerateInstanceExtensionProperties(null,count&,exts)

	//printf("%i : %s\n",i,exts[^i].extensionName)

	//wantedLayers := AVLSet.{string}()
	//wantedLayers << "VK_LAYER_LUNARG_standard_validation"
	//
	//toUseLayers := Queue.{string}()
	//if wantedLayers.Contain(lays[^].layerName)
	//	toUseLayers.Push(it.layerName)

	//printf("using %i %s\n",i,toUseLayers[^i])

	//dlclose(handl)
	return 0
}

