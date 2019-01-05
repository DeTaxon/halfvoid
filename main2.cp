

vkLoadAddr := PFN_vkGetInstanceProcAddr
vkEnumerateInstanceExtensionProperties := PFN_vkEnumerateInstanceExtensionProperties
vkEnumerateInstanceLayerProperties := PFN_vkEnumerateInstanceLayerProperties
vkCreateInstance := PFN_vkCreateInstance

main := !(int argc, char^^ argv) -> int
{

	t := new Queue.{int}[2]
	t[0].Push(1)
	t[0].Push(6)
	t[1].Push(10)
	t[1].Push(-3)
	t[1].Push(7)

	printf("hey %i %i %i\n",t[^i][^j],i,j)

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

	//for i : lays->len printf("%i : %s\n",i,lays[i].layerName)

	//vkEnumerateInstanceExtensionProperties(null,count&,null)
	//exts := new VkExtensionProperties[count] ; $temp
	//vkEnumerateInstanceExtensionProperties(null,count&,exts)

	//for i : count printf("%i : %s\n",i,exts[i].extensionName)

	//wantedLayers := AVLSet.{string}()
	//wantedLayers << "VK_LAYER_LUNARG_standard_validation"

	//
	//toUseLayers := Queue.{string}()
	//for lays
	//	if wantedLayers.Contain(it.layerName)
	//		toUseLayers.Push(it.layerName)

	//for it : toUseLayers, i : 0  printf("using %i %s\n",i,it)

	//dlclose(handl)
	return 0
}

