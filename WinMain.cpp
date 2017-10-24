#ifdef WINPROJ

#include <windows.h>

MSG msg;
int QuitMsg;
HINSTANCE VKConnect;
HWND VKWin;

void (*key_event)(int key, int action);

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	// case WM_PAINT:
	//     // The validation callback calls MessageBox which can generate paint
	//     // events - don't make more Vulkan calls if we got here from the
	//     // callback
	//     if (!in_callback) {
	//         demo_run(&demo);
	//     }
	//     break;
	// case WM_GETMINMAXINFO:     // set window's minimum size
	//     ((MINMAXINFO*)lParam)->ptMinTrackSize = demo.minsize;
	//     return 0;
	// case WM_SIZE:
	//     // Resize the application to the new window size, except when
	//     // it was minimized. Vulkan doesn't support images or swapchains
	//     // with width=0 and height=0.
	//     if (wParam != SIZE_MINIMIZED) {
	//         demo.width = lParam & 0xffff;
	//         demo.height = (lParam & 0xffff0000) >> 16;
	//         demo_resize(&demo);
	//     }
	//     break;
	default:
		break;
	}
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

extern "C" void TCreateWindow(int w,int h)
{
	WNDCLASSEX win_class;

	// Initialize the window class structure:
	win_class.cbSize = sizeof(WNDCLASSEX);
	win_class.style = CS_HREDRAW | CS_VREDRAW;
	win_class.lpfnWndProc = WndProc;
	win_class.cbClsExtra = 0;
	win_class.cbWndExtra = 0;
	win_class.hInstance = VKConnect; // hInstance
	win_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	win_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	win_class.lpszMenuName = NULL;
	win_class.lpszClassName = AppName;
	win_class.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
	// Register window class:
	if (!RegisterClassEx(&win_class)) {
		exit(1);
	}
	// Create window with the registered class:
	RECT wr = {0, 0, w, h};
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
	VKWin = CreateWindowEx(0,
	                       AppName,           // class name
	                       AppName,           // app name
	                       WS_OVERLAPPEDWINDOW | // window style
	                       WS_VISIBLE | WS_SYSMENU,
	                       100, 100,           // x/y coords
	                       wr.right - wr.left, // width
	                       wr.bottom - wr.top, // height
	                       NULL,               // handle to parent
	                       NULL,               // handle to menu
	                       VKConnect,          // hInstance
	                       NULL);              // no extra parameters
	if (!VKWin) {
		exit(1);
	}
}

extern "C" void PullEvents()
{
	PeekMessage(&msg, nullptr, 0,0,PM_REMOVE);
	if(msg.message == WM_QUIT)
	{
		NotClosed = false;
		QuitMsg = msg.wParam;
	}else
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

extern "C" void TDestroyWindow()
{
	DestroyWindow(VKWin);
}

extern "C" main(int,char**);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine,int nCmdShow) 
{
	VKConnect = hInstance;

	main(0,nullptr);

	return QuitMsg;
}

#endif
