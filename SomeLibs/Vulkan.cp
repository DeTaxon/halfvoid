
//#include <dlfcn.h>
//macro GlobalFunc = {vkCreateInstance,vkEnumerateInstanceExtensionProperties,vkEnumerateInstanceLayerProperties}
//
//macro SpecFunc = {vkCreateXcbSurfaceKHR};
//
//macro InstFunc = {vkDestroyInstance ,vkEnumeratePhysicalDevices, vkGetPhysicalDeviceProperties,vkGetPhysicalDeviceFeatures,vkGetPhysicalDeviceQueueFamilyProperties
//		,vkCreateDevice,vkGetDeviceProcAddr,vkGetDeviceQueue,vkDeviceWaitIdle,vkCreateDebugReportCallbackEXT,vkDebugReportMessageEXT,vkDestroyDebugReportCallbackEXT
//		,vkDestroyDevice,vkDestroySurfaceKHR,vkEnumerateDeviceExtensionProperties,vkCreateSwapchainKHR,vkDestroySwapchainKHR,vkGetSwapchainImagesKHR,vkAcquireNextImageKHR
//		,vkQueuePresentKHR,vkGetPhysicalDeviceSurfacePresentModesKHR,vkGetPhysicalDeviceSurfaceFormatsKHR,vkGetPhysicalDeviceSurfaceCapabilitiesKHR
//		,vkGetPhysicalDeviceSurfaceSupportKHR,vkCreateCommandPool,vkAllocateCommandBuffers,vkBeginCommandBuffer,vkCmdPipelineBarrier,vkCmdClearColorImage,vkEndCommandBuffer
//		,vkCreateSemaphore,vkQueueSubmit,vkCreateRenderPass,vkCreateImageView,vkCreateFramebuffer,vkCreateShaderModule,vkCreatePipelineLayout,vkCreateGraphicsPipelines
//		,vkCmdBeginRenderPass,vkCmdBindPipeline,vkCmdDraw,vkCmdEndRenderPass,vkCreateBuffer,vkDestroyBuffer,vkGetBufferMemoryRequirements
//		,vkGetPhysicalDeviceMemoryProperties,vkAllocateMemory,vkBindBufferMemory,vkMapMemory,vkFlushMappedMemoryRanges,vkUnmapMemory,vkCmdBindVertexBuffers
//		,vkResetCommandBuffer,vkCreateFence,vkWaitForFences,vkResetFences,vkCmdDrawIndexed,vkCmdBindIndexBuffer,vkCreateDescriptorSetLayout,vkCreateDescriptorPool
//		,vkUpdateDescriptorSets,vkCmdBindDescriptorSets,vkAllocateDescriptorSets,vkFreeMemory,vkDestroyDescriptorPool,vkDestroyShaderModule,vkDestroyImageView
//		,vkDestroyFramebuffer,vkDestroyFence,vkDestroySemaphore,vkDestroyPipeline,vkDestroyRenderPass,vkDestroyCommandPool,vkDestroyPipelineLayout
//		,vkDestroyDescriptorSetLayout}

VkExtensionPropertie := class {
    extensionName := char[VK_MAX_EXTENSION_NAME_SIZE]
    specVersion := u32
} 

VK_LOD_CLAMP_NONE                ::  1000.0
VK_TRUE                          ::  1
VK_FALSE                         ::  0
VK_MAX_PHYSICAL_DEVICE_NAME_SIZE ::  256
VK_UUID_SIZE                     ::  16
VK_MAX_MEMORY_TYPES              ::  32
VK_MAX_MEMORY_HEAPS              ::  16
VK_MAX_EXTENSION_NAME_SIZE       ::  256
VK_MAX_DESCRIPTION_SIZE          ::  256

VulkanLibrary := void^
vkGetInstanceProcAddr := !(void^ inst, char^)^ -> void^

dlopen := !(char^ name,int flags) -> void^ declare
dlsym := !(void^ lib, char^ name)-> void^ declare

RTLD_NOW :: 2

vkEnumerateInstanceExtensionProperties := !(void^ inst, u32^ x, void^ arr)^ -> void 

InitVulkan := !() -> void
{
	VulkanLibrary = dlopen("./libvulkan.so.1.0.39",RTLD_NOW)
	if(VulkanLibrary) vkGetInstanceProcAddr = dlsym(VulkanLibrary,"vkGetInstanceProcAddr")
	if(VulkanLibrary) vkEnumerateInstanceExtensionProperties = dlsym(VulkanLibrary,"vkEnumerateInstanceExtensionProperties")
}
		//Func = (PFN_$$Func)vkGetInstanceProcAddr(nullptr,Func$str);
 
InitInstance := !() -> bool
{
	ExtCount := u32
	Lays := u32

	//vkEnumerateInstanceExtensionProperties(null,ExtCount&,null)

	if ExtCount->{s32}
	{
		arr := new VkExtensionPropertie[ExtCount->{s32}]
		//vkEnumerateInstanceExtensionProperties(null,ExtCount&,arr)

		printf("--Extensions--\n")		
		for ExtCount->{s32}
		{	
			//printf("%s\n",arr[it].extensionName)
		}
	}

	//vkEnumerateInstanceLayerProperties(&Lays, nullptr);

	//vector<VkLayerProperties> arLays(Lays);
	//if(Lays)
	//{
	//	vkEnumerateInstanceLayerProperties(&Lays, arLays.data());

	//	cout<<"--Layers--\n";
	//	for( auto &c: arLays)
	//	{
	//		cout<<c.layerName<<'\n';
	//	}
	//}

	//VkInstanceCreateInfo ToCreate = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,	nullptr, 0 , &AppInfo, 0 , nullptr, 0 ,nullptr	};

	//if(NeedExts$size)
	//{
	//	ToCreate.enabledExtensionCount = NeedExts$size;
	//	ToCreate.ppEnabledExtensionNames = NeedExts;
	//}

	//if(NeedLays$size)
	//{
	//	ToCreate.enabledLayerCount = NeedLays$size;
	//	ToCreate.ppEnabledLayerNames = NeedLays;
	//	//ToCreate.enabledLayerCount = 0;
	//	//ToCreate.ppEnabledLayerNames = nullptr;
	//}
	//if(vkCreateInstance(&ToCreate,nullptr,&vkInst) != VK_SUCCESS)
	//{
	//	cout<<"Can't create instance\n";
	//	return false;
	//}
	return true	
}
