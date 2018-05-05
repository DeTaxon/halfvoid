#import "wayland-lib.cp"

g_w_started := bool
g_w_display := void^

WaylandWindow := class 
{
	 start := !() -> void
	 {
	 	if not g_w_started
		{
			g_w_display = wl_display_connect(null)
			printf("wow %p\n",g_w_display)
		}
	 }
}
