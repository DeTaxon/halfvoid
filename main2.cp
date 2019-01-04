

vkLoadAddr := PFN_vkGetInstanceProcAddr
vkEnumerateInstanceExtensionProperties := PFN_vkEnumerateInstanceExtensionProperties
vkEnumerateInstanceLayerProperties := PFN_vkEnumerateInstanceLayerProperties
vkCreateInstance := PFN_vkCreateInstance

main := !(int argc, char^^ argv) -> int
{

	t := Queue.{int}()
	t.Push(1)
	t.Push(6)
	t.Push(10)
	t.Push(-3)
	t.Push(7)
	
	for it, i : t
		printf("%i heh %i\n",i,it)

	h := AVLMap.{string,int}()
	h["456"] = 5
	h["724"] = 0
	h["dsgsdfg"] = 13

	for it, i : h
		printf("hah %s %i\n", i,it)

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

