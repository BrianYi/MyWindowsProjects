#include <Windows.h>

#pragma comment(lib, "winmm.lib")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// CS   Class style option
// CW   Create window option
// DT   Draw text option
// IDI  ID number for an icon
// IDC  ID number for a cursor
// MB   Message box options
// SND  Sound option
// WM   Window message
// WS   Window style

int WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd )
{
    static TCHAR szAppName[] = TEXT("HelloWin");
    HWND        hwnd;       // Handle to a window
    MSG         msg;        // Message structure
    WNDCLASS    wndclass;   // Window class structure

    wndclass.style          = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc    = WndProc;
    wndclass.cbClsExtra     = 0;
    wndclass.cbWndExtra     = 0;
    wndclass.hInstance      = hInstance;
    wndclass.hIcon          = LoadIcon(0, IDI_APPLICATION);
    wndclass.hCursor        = LoadCursor(0, IDC_ARROW);
    wndclass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName   = 0;
    wndclass.lpszClassName  = szAppName;
    
    if (!RegisterClass(&wndclass)) {
        MessageBox(0, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
        return 0;
    }
    hwnd = CreateWindow(szAppName,
        TEXT("The Hello Program"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        0,
        0,
        hInstance,
        0);
    ShowWindow(hwnd, nShowCmd);
    UpdateWindow(hwnd);         // send a WM_PAINT message to window procedure

	BOOL fRet;
    while( (fRet = GetMessage(&msg, 0, 0, 0)) != 0) {
		if (fRet == -1) {
			// handle the error and possibly exit
			break;
		}
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    	{
            PlaySound(TEXT("hellowin.wav"), 0, SND_FILENAME | SND_ASYNC);
            return 0;
    	}
    case WM_PAINT:
		{ 
			LOGFONT logfont;
			::ZeroMemory(&logfont, sizeof(LOGFONT));
			logfont.lfCharSet = GB2312_CHARSET;
			logfont.lfHeight = 20;
			HFONT hFont = CreateFontIndirect(&logfont);

			HDC hdc = GetDC(0);
			RECT        rect; 
			rect.left = rect.top = 0;
			rect.right = rect.bottom = 200;
			SetTextColor(hdc, RGB(0,0,255));
			SetBkColor(hdc, RGB(255,0,0));
			SetBkMode(hdc, OPAQUE);
			SelectObject(hdc, hFont);
			
			//GetClientRect(hwnd, &rect);

			DrawText(hdc, TEXT("Hello, Windows 98!"), -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			ReleaseDC(0, hdc);
			DeleteObject(hFont);
// 			HDC         hdc;    // Handle to a device context
// 			PAINTSTRUCT ps;     // Paint structure
// 			RECT        rect;   // Rectangle structure
// 			hdc = BeginPaint(hwnd, &ps);	// invalid region to be erased
//             GetClientRect(hwnd, &rect);
// 
//             DrawText(hdc, TEXT("Hello, Windows 98!"), -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
//             EndPaint(hwnd, &ps);	// release the device context handle
// 
// 			SelectClipPath(hdc, RGN_AND);
// 			FillRect(hdc, &rect, (HBRUSH)GetStockObject(GRAY_BRUSH));
            return 0;
        }
    case WM_DESTROY:
        {
            PostQuitMessage(5);
            return 0;
        }
	case WM_CHAR:
		{
			// deal with input operation
			return 0;
		}
    default:
    	break;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}