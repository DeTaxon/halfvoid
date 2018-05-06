//
// WAYLAND CORE
//

wl_event_queue_destroy := !(void^ queue) -> void declare
wl_proxy_marshal := !(void^ p, u32 opcode, ...) -> void declare
wl_proxy_marshal_array := !(void^ p, u32 opcode,void^ args) -> void declare
wl_proxy_create := !(void^ factory,void^ interface) -> void^ declare

wl_proxy_create_wrapper := !(void^ proxy) -> void^ declare
wl_proxy_wrapper_destroy := !(void^ proxy_wrapper) -> void declare
wl_proxy_marshal_constructor := !(void^ proxy, s32 opcode, 
			void^ interface, ...) -> void^ declare
wl_proxy_marshal_constructor_versioned := !(void^ proxy,u32 opcode,
			void^ interface, u32 version, ...) -> void^ declare

wl_proxy_marshal_array_constructor:= !(void^ proxy,
				   u32 opcode, void* args,
				   void^ interface) -> void^ declare

wl_proxy_marshal_array_constructor_versioned := !(void^ proxy,
					     u32 opcode,
					     void^ args,
					     void^ interface,
					     u32 version) -> void^ declare

wl_proxy_destroy := !(void^ proxy) -> void declare

//wl_proxy_add_listener := !(void^ proxy,
//		      void (**implementation)(void), void *data) -> int
wl_proxy_add_listener := !(void^ proxy,void^ implementation, void^ data) -> int declare

wl_proxy_get_listener := !(void^ proxy) -> void^ declare

//wl_proxy_add_dispatcher := !(void^ proxy,
//			wl_dispatcher_func_t dispatcher_func,
//			void^ dispatcher_data, void^ data) -> int

wl_proxy_set_user_data := !(void^ proxy, void^ user_data) -> void declare
wl_proxy_get_user_data := !(void^ proxy) -> void^ declare
wl_proxy_get_version := !(void^ proxy) -> u32 declare
wl_proxy_get_id := !(void^ proxy) -> u32 declare
wl_proxy_get_class := !(char^ proxy) -> char^ declare
wl_proxy_set_queue := !(void^ proxy, void^ queue) -> void declare
wl_display_connect := !(char^ name) -> void^ declare
wl_display_connect_to_fd := !(int fd) -> void^ declare
wl_display_disconnect := !(void^ display) -> void declare
wl_display_get_fd := !(void^ display) -> int declare
wl_display_dispatch := !(void^ display) -> int declare
wl_display_dispatch_queue := !(void^ display, void^ queue) -> int declare
wl_display_dispatch_queue_pending := !(void^ display, void^ queue) -> int declare
wl_display_dispatch_pending := !(void^ display) -> int declare
wl_display_get_error := !(void^ display) -> int declare
wl_display_get_protocol_error := !(void^ display,void^^ interface,u32^ id) -> u32 declare
wl_display_flush := !(void^ display) -> int declare
wl_display_roundtrip_queue := !(void^ display,  void^ queue) -> int declare
wl_display_roundtrip := !(void^ display) -> int declare
wl_display_create_queue := !(void^ display) -> void^ declare
wl_display_prepare_read_queue := !(void^ display,  void^ queue) -> int declare
wl_display_prepare_read := !(void^ display) -> int declare
wl_display_cancel_read := !(void^ display) -> void declare
wl_display_read_events := !(void^ display) ->int declare
//wl_log_set_handler_client := !(wl_log_func_t handler) -> void declare


//
// WAYLAND UTILS
//

wl_interface := class
{
	name := char^
	version := s32
	method_count := s32
	methods := void^ //wl_message^
	event_count := s32
	events := void^ //wl_message^
}

wl_message := class
{
	name := char^
	signature := char^
	types := wl_interface^^
}


//
// WAYLAND PROTOCOL
//


wl_display_interface := extern opaque
wl_registry_interface := extern opaque
wl_callback_interface := extern opaque
wl_compositor_interface := opaque
wl_shm_pool_interface := opaque
wl_shm_interface := opaque
wl_buffer_interface := opaque
wl_data_offer_interface := opaque
wl_data_source_interface := opaque
wl_data_device_interface := opaque
wl_data_device_manager_interface := opaque
wl_shell_interface := opaque
wl_shell_surface_interface := opaque
wl_surface_interface := opaque
wl_seat_interface := opaque
wl_pointer_interface := opaque
wl_keyboard_interface := opaque
wl_touch_interface := opaque
wl_output_interface := opaque
wl_region_interface := opaque
wl_subcompositor_interface := opaque
wl_subsurface_interface := opaque

WL_DISPLAY_ERROR_INVALID_OBJECT := 0
WL_DISPLAY_ERROR_INVALID_METHOD := 1
WL_DISPLAY_ERROR_NO_MEMORY := 2

wl_display_listener  := class
{
	//error :=  !(void^ data,
	//	      void^ wl_display,
	//	      void^ object_id,
	//	      u32 code,
	//	      char^ message)^ -> void
	//delete_id := !(void^ data,void^ wl_display, u32 id)^ -> void
	error := void^
	delete_id := void^
}

WL_DISPLAY_SYNC  := 0
WL_DISPLAY_GET_REGISTRY  := 1

wl_display_get_registry := !(void^ wl_display) -> void^
{
	return wl_proxy_marshal_constructor( wl_display, WL_DISPLAY_GET_REGISTRY, wl_registry_interface&, null)
}

wl_display_add_listener := !(void^ wl_display,void^ listener, void^ data) -> int
{
	return wl_proxy_add_listener(wl_display,listener, data)
}

wl_registry_add_listener := !(void^ wl_registry,void^ listener, void^ data) -> int
{
	return wl_proxy_add_listener(wl_registry,listener, data)
}

WL_REGISTRY_BIND := 0
wl_registry_bind := !(void^ reg, u32 name, void^ interface, u32 version) -> void^
{
	
	return wl_proxy_marshal_constructor_versioned( reg, WL_REGISTRY_BIND, interface, version, name, interface.name, version, null)
}

WL_COMPOSITOR_CREATE_SURFACE  := 0
WL_COMPOSITOR_CREATE_REGION := 1

wl_compositor_create_surface := !(void^ comp) -> void^
{
	return wl_proxy_marshal_constructor(comp, WL_COMPOSITOR_CREATE_SURFACE, wl_surface_interface&, null)
}

WL_SHELL_SURFACE_PONG  := 0
WL_SHELL_SURFACE_MOVE  := 1
WL_SHELL_SURFACE_RESIZE  := 2
WL_SHELL_SURFACE_SET_TOPLEVEL  := 3
WL_SHELL_SURFACE_SET_TRANSIENT  := 4
WL_SHELL_SURFACE_SET_FULLSCREEN  := 5
WL_SHELL_SURFACE_SET_POPUP  := 6
WL_SHELL_SURFACE_SET_MAXIMIZED  := 7
WL_SHELL_SURFACE_SET_TITLE  := 8
WL_SHELL_SURFACE_SET_CLASS  := 9

WL_SHELL_GET_SHELL_SURFACE  := 0

wl_shell_get_shell_surface := !(void^ wl_shell, void^ surface) -> void^
{
	return wl_proxy_marshal_constructor( wl_shell,
			 WL_SHELL_GET_SHELL_SURFACE, wl_shell_surface_interface&, null, surface)

}
wl_shell_surface_set_toplevel := !(void^ wl_shell_surface) -> void
{
	wl_proxy_marshal(wl_shell_surface, WL_SHELL_SURFACE_SET_TOPLEVEL)
}
