#import "lib.cp"
#import "wayland.cp"

getch := !() -> void declare
//vkEnumerateInstanceExtensionProperties := !(

VkLayerProperties := class
{
	name := char[256]
	version := s32
	iVer := s32
	descr := char[256]
}
vkEnumerateInstanceLayerProperties := !(int^ , VkLayerProperties^)^ -> void


global_registry_handler := !( void^ data, void^ reg, u32 id, char^ intr, u32 version) -> void
{
	printf("hey %s\n",intr)
}

remover := !(void^ data, void^ reg, u32 id) -> void
{
	
}

listt := wl_display_listener

main := !(int argc, char^^ argv) -> int
{

	listt.error = global_registry_handler
	listt.delete_id = remover

	g_w_display := wl_display_connect(null)
	
	if g_w_display == null return 0

	reg := wl_display_get_registry(g_w_display)
	wl_registry_add_listener(reg,listt&,null)

	wl_display_dispatch(g_w_display)
	wl_display_roundtrip(g_w_display)

	getch()

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





