#include <xcb/xcb.h>
#include <stdio.h>
#include <stdlib.h>


bool Opend = true;

bool IsOpen() { return Opend;}

bool Keys[256];

xcb_connection_t* vkConnect;
xcb_window_t vkWin;
xcb_screen_t* screen;
xcb_atom_t wmProtocols;
xcb_atom_t wmDeleteWin;

void (*key_event)(int key, int action);

extern "C" void GetConnection() {
    const xcb_setup_t *setup;
    xcb_screen_iterator_t iter;
    int scr;

    vkConnect = xcb_connect(NULL, &scr);
    if (xcb_connection_has_error(vkConnect) > 0) {
        Opend = false;
	return;
    }

    setup = xcb_get_setup(vkConnect);
    iter = xcb_setup_roots_iterator(setup);
    while ((scr--) > 0)
        xcb_screen_next(&iter);

    screen = iter.data;
}

void LikeWndProc(const xcb_generic_event_t *event) 
{
    uint8_t event_code = event->response_type & 0x7F;
    const xcb_key_release_event_t *key =  (const xcb_key_release_event_t *)event;
    char Line1[] = "qwertyuiop[]";
    char Line2[] = "asdfghjkl;'";
    int KeyC;
    switch (event_code) {
    case XCB_EXPOSE:
        break;
    case XCB_CLIENT_MESSAGE:
        if ((*(xcb_client_message_event_t *)event).data.data32[0] ==
            wmDeleteWin) {
            break;
        }
        break;
    case XCB_KEY_PRESS:
	if (!key_event) break;
	KeyC = (int)key->detail;
	if(KeyC >= 24 && KeyC <= 35) key_event(Line1[KeyC - 24],1); 
	if(KeyC >= 38 && KeyC <= 48) key_event(Line2[KeyC - 38],1); 
	break;
    case XCB_KEY_RELEASE: 
	if (!key_event) break;
	KeyC = (int)key->detail;
	if(KeyC >= 24 && KeyC <= 35) key_event(Line1[KeyC - 24],0); 
	if(KeyC >= 38 && KeyC <= 48) key_event(Line2[KeyC - 38],0);
     break;
    }
}

extern "C" void SetKeyEvent( void (*event)(int,int))
{
	key_event = event;
}

extern "C" void TCreateWindow(int w,int h) 
{	

    GetConnection();

    uint32_t value_mask, value_list[32];

    GetConnection();
    vkWin = xcb_generate_id(vkConnect);

    value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    value_list[0] = screen->black_pixel;
    value_list[1] =  XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_KEY_PRESS;// | XCB_EVENT_MASK_EXPOSURE |  XCB_EVENT_MASK_STRUCTURE_NOTIFY;

    xcb_create_window(vkConnect, XCB_COPY_FROM_PARENT, vkWin,
                      screen->root, 0, 0, w, h, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual,
                      value_mask, value_list);

    /* Magic code that will send notification when window is destroyed */
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(vkConnect, 0, 12, "WM_PROTOCOLS");
    xcb_intern_atom_cookie_t cookie2 = xcb_intern_atom(vkConnect, 0, 16, "WM_DELETE_WINDOW");
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(vkConnect, cookie, 0);
    xcb_intern_atom_reply_t *reply2 = xcb_intern_atom_reply(vkConnect, cookie2, 0);

    wmDeleteWin = reply->atom;
    wmProtocols= reply2->atom;

    xcb_change_property(vkConnect, XCB_PROP_MODE_REPLACE, vkWin,
                        (*reply).atom, 4, 32, 1,&((*reply2).atom));
    free(reply);
    free(reply2);

    xcb_map_window(vkConnect,vkWin);
    xcb_flush(vkConnect);

    // Force the x/y coordinates to 100,100 results are identical in consecutive
    // runs
    const uint32_t coords[] = {100, 100};
    xcb_configure_window(vkConnect, vkWin,XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, coords);
}

extern "C" void PullEvents()
{
	xcb_generic_event_t* event =  xcb_poll_for_event(vkConnect);
	while(event)
	{
		LikeWndProc(event);
		free(event);
		event =  xcb_poll_for_event(vkConnect);
	}
}

extern "C" void TDestroyWindow()
{
	xcb_destroy_window(vkConnect,vkWin);
}

