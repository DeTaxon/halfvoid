
vkLoadAddr := PFN_vkGetInstanceProcAddr
vkEnumerateInstanceExtensionProperties := PFN_vkEnumerateInstanceExtensionProperties
vkEnumerateInstanceLayerProperties := PFN_vkEnumerateInstanceLayerProperties
vkCreateInstance := PFN_vkCreateInstance

vkDllHandle := void^

InitVulkan := !() -> bool
{
	vkDllHandle = dlopen("libvulkan.so.1",2)

	vkLoadAddr = dlsym(vkDllHandle,"vkGetInstanceProcAddr")

	if vkLoadAddr == null{
		printf("cant get function ProcAddr\n")
		return 0
	}

	newFunc := void^
	newFunc = vkLoadAddr(null,"vkEnumerateInstanceExtensionProperties")
	if newFunc == null{ printf("cant get func props\n")	return 0	}
	vkEnumerateInstanceExtensionProperties = newFunc->{PFN_vkEnumerateInstanceExtensionProperties}

	newFunc = vkLoadAddr(null,"vkEnumerateInstanceLayerProperties")
	if newFunc == null{ printf("cant get func props\n")	return 0	}
	vkEnumerateInstanceLayerProperties = newFunc->{PFN_vkEnumerateInstanceLayerProperties}

	newFunc = vkLoadAddr(null,"vkCreateInstance")
	if newFunc == null{ printf("cant get func props\n")	return 0	}
	vkCreateInstance = newFunc->{PFN_vkCreateInstance}

	count := s32

	vkEnumerateInstanceLayerProperties(count&,null)
	lays := new VkLayerProperties[count] ; $temp
	vkEnumerateInstanceLayerProperties(count&,lays)

	printf("%i : %s\n",i,lays[^i].layerName)

	vkEnumerateInstanceExtensionProperties(null,count&,null)
	exts := new VkExtensionProperties[count] ; $temp
	vkEnumerateInstanceExtensionProperties(null,count&,exts)

	printf("%i : %s\n",i,exts[^i].extensionName)

	wantedLayers := AVLSet.{string}() ; $temp
	wantedLayers << "VK_LAYER_LUNARG_standard_validation"
	
	toUseLayers := Queue.{string}() ; $temp
	if wantedLayers.Contain(lays[^].layerName)
		toUseLayers.Push(it.layerName)

	printf("using %i %s\n",i,toUseLayers[^i])

	return 0
}

DestroyVulkan := !() -> void
{
	dlclose(vkDllHandle)
}
