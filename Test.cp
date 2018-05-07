#import "lib.cp"
#import "wayland.cp"
#import "egl.cp"
#import "gl.cp"

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


	region := wl_compositor_create_region(compos)
	wl_region_add(region,0,0,400,400)
	wl_surface_set_opaque_region(surf,region)


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

	egl_win := wl_egl_window_create(surf,400,400)
	
	egl_surf := eglCreateWindowSurface(egl_display,confs[0],egl_win,null)
	eglMakeCurrent(egl_display,egl_surf,egl_surf,egl_cont)

	
	
		glClearColor(1.0,0.5,0.0,1.0)
		glClear(GL_COLOR_BUFFER_BIT)
		glFlush()
		eglSwapBuffers(egl_display,egl_surf)

	wl_display_dispatch(g_w_display)
		system("sleep 1s")
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





