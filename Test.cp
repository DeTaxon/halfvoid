#import "lib.cp"
#import "wayland.cp"

//vkEnumerateInstanceExtensionProperties := !(

strcmp := !(string a,string b) -> int declare
system := !(char^ cmd) -> void declare

VkLayerProperties := class
{
	name := char[256]
	version := s32
	iVer := s32
	descr := char[256]
}
vkEnumerateInstanceLayerProperties := !(int^ , VkLayerProperties^)^ -> void

compos := void^
shell := void^

global_registry_handler := !( void^ data, void^ reg, u32 id, char^ intr, u32 version) -> void
{
	//printf("hey %s %i\n",intr,version)
	
	if strcmp(intr,"wl_compositor") == 0
	{
		compos = wl_registry_bind(reg,id,wl_compositor_interface&,1)
	}
	//if strcmp(intr,"zxdg_shell_v6") == 0
	if strcmp(intr,"wl_shell") == 0
	{
		shell = wl_registry_bind(reg,id,wl_shell_interface&,1)
	}
}

remover := !(void^ data, void^ reg, u32 id) -> void
{
	printf("wut %i\n",id)	
}

listt := wl_display_listener

main := !(int argc, char^^ argv) -> int
{

	listt.error = global_registry_handler
	listt.delete_id = remover

	g_w_display := wl_display_connect(null)
	
	//if g_w_display == null return 0 ???

	reg := wl_display_get_registry(g_w_display)
	wl_registry_add_listener(reg,listt&,null)

	wl_display_dispatch(g_w_display)
	wl_display_roundtrip(g_w_display)

	surf := wl_compositor_create_surface(compos)
	shell_surf := wl_shell_get_shell_surface(shell,surf)

	wl_shell_surface_set_toplevel(shell_surf)	

	system("sleep 4s")
	wl_display_disconnect(g_w_display)
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





