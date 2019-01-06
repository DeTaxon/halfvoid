
vkLoadAddr := PFN_vkGetInstanceProcAddr
vkEnumerateInstanceExtensionProperties := PFN_vkEnumerateInstanceExtensionProperties
vkEnumerateInstanceLayerProperties := PFN_vkEnumerateInstanceLayerProperties
vkCreateInstance := PFN_vkCreateInstance


val := int


testFunc := !(void^ itm) -> void^
{
	for i : 40000
	{
		#critical_atomic
		val += 1
	}
	return null
}


main := !(int argc, char^^ argv) -> int
{
	thr1 := new Thread(testFunc)
	thr2 := new Thread(testFunc)
	thr3 := new Thread(testFunc)
	thr1.Join()
	thr2.Join()
	thr3.Join()
	printf("total %i\n",val)
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

