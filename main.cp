#import "lib.cp"
#import "wayland.cp"
#import "egl.cp"
#import "gl.cp"

//vkEnumerateInstanceExtensionProperties := !(

working := bool

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
xdg_item := void^

le_pong := !(void^ data, void^ xdg_base, u32 serial) -> void
{
	//xdg_wm_base_pong(xdg_base,serial)
	zxdg_shell_v6_pong(xdg_base,serial)
}
pingList := xdg_wm_base_listener

global_registry_handler := !( void^ data, void^ reg, u32 id, char^ intr, u32 version) -> void
{
	printf("hey %s %i\n",intr,version)
	
	if strcmp(intr,"wl_compositor") == 0
	{
		compos = wl_registry_bind(reg,id,wl_compositor_interface&,1)
	}
	//if strcmp(intr,"xdg_shell") == 0
	//{
	//	xdg_item = wl_registry_bind(reg,id,zxdg_shell_v6_interface&,1)
	//}
	if strcmp(intr,"zxdg_shell_v6") == 0
	{
		xdg_item = wl_registry_bind(reg,id,xdg_wm_base_interface&,1)
	zxdg_shell_v6_add_listener(xdg_item,pingList&,null)
	}
	
	if strcmp(intr,"wl_shell") == 0
	{
		//shell = wl_registry_bind(reg,id,xdg_wm_base_interface&,1)
		shell = wl_registry_bind(reg,id,wl_shell_interface&,1)
	}
}

remover := !(void^ data, void^ reg, u32 id) -> void
{
}


conf_wut := !(void^ data, void^ top_lvl, s32 widt, s32 hei, void^ state) -> void
{
	printf("new size %i %i\n",widt,hei)
}
close_wut := !(void^ data, void^ tp_lvl) -> void
{
	working = false
}

shell_handle := !(void^ data, void^ surf, u32 serial) -> void
{
	zxdg_surface_v6_ack_configure(surf,serial)
}

listt := wl_display_listener
resizeList := zxdg_toplevel_v6_listener
cfgLi := xdg_wm_base_listener

main2 := !(int argc, char^^ argv) -> int
{
	working = true
	pingList.ping = le_pong
	listt.error = global_registry_handler
	listt.delete_id = remover
	resizeList.configure = conf_wut
	resizeList.close = close_wut
	cfgLi.ping = shell_handle

	g_w_display := wl_display_connect(null)
	
	//if g_w_display == null return 0 ???

	reg := wl_display_get_registry(g_w_display)
	wl_registry_add_listener(reg,listt&,null)

	wl_display_dispatch(g_w_display)
	wl_display_roundtrip(g_w_display)


	surf := wl_compositor_create_surface(compos)
	//shell_surf := wl_shell_get_shell_surface(shell,surf)
	//xdg_surf := xdg_wm_base_get_xdg_surface(xdg_item,surf)
	xdg_surf := zxdg_shell_v6_get_xdg_surface(xdg_item,surf)

	zxdg_surface_v6_add_listener(xdg_surf,cfgLi&,null)

	wl_shell_surface_set_toplevel(xdg_surf)
	//xdg_level := xdg_surface_get_toplevel(xdg_surf)
	xdg_level := zxdg_surface_v6_get_toplevel(xdg_surf)
	zxdg_toplevel_v6_add_listener(xdg_level,resizeList&,null)
	zxdg_toplevel_v6_set_title(xdg_level,"hello pls")

	zxdg_surface_v6_set_window_geometry(xdg_surf,20,20,700,700)

	wl_surface_commit(surf)


	egl_display := eglGetDisplay(g_w_display)

	attrs := new int[20]

	attrs[0] = EGL_SURFACE_TYPE
	attrs[1] = EGL_WINDOW_BIT
	attrs[2] = EGL_RED_SIZE
	attrs[3] = 8
	attrs[4] = EGL_GREEN_SIZE
	attrs[5] = 8
	attrs[6] = EGL_BLUE_SIZE
	attrs[7] = 8
	attrs[8] = EGL_RENDERABLE_TYPE
	attrs[9] = EGL_OPENGL_ES2_BIT
	attrs[10] = EGL_DEPTH_SIZE
	attrs[11] = 24
	attrs[12] = EGL_NONE

	conattr := new int[3]
	conattr[0] = EGL_CONTEXT_CLIENT_VERSION
	conattr[1] = 2
	conattr[2] = EGL_NONE

	maj := int
	min := int
	isInitedEGL := eglInitialize(egl_display,maj&,min&)
	printf("egl %i.%i isInited %i\n",maj,min,isInitedEGL)
	
	cCount := int
	eglGetConfigs(egl_display,null,0,cCount&)

	if cCount > 100
	{
		printf("nah\n")
		return 0
	}

	confs := new void^[cCount]
	//eglGetConfigs(egl_display,confs,cCount,cCount&)

	eglChooseConfig(egl_display,attrs,confs,cCount,cCount&)

	sizeI := int
	redI := int
	for i : cCount
	{
		eglGetConfigAttrib(egl_display,confs[i],EGL_BUFFER_SIZE,sizeI&)
		eglGetConfigAttrib(egl_display,confs[i],EGL_RED_SIZE,redI&)
		printf("Buffer %i %i %i\n",i,sizeI,redI)

	}

	egl_cont := eglCreateContext(egl_display,confs[0],null,conattr)

	egl_win := wl_egl_window_create(surf,700,700)
	
	egl_surf := eglCreateWindowSurface(egl_display,confs[0],egl_win,null)
	eglMakeCurrent(egl_display,egl_surf,egl_surf,egl_cont)

	
	
		glClearColor(1.0,0.5,0.0,1.0)
		glClear(GL_COLOR_BUFFER_BIT)
		glFlush()
		eglSwapBuffers(egl_display,egl_surf)
	while working
	{
		wl_display_dispatch(g_w_display)
	}
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





