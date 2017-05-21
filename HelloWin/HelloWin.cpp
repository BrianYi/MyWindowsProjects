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
    HWND hwndChild = CreateWindow(szAppName,
        TEXT("Child window"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        hwnd,
        0,
        hInstance,
        0);
    ShowWindow(hwnd, nShowCmd);
    ShowWindow(hwndChild, SW_MAXIMIZE);
    UpdateWindow(hwnd);         // send a WM_PAINT message to window procedure
    UpdateWindow(hwndChild);

    while(GetMessage(&msg, 0, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC         hdc;    // Handle to a device context
    PAINTSTRUCT ps;     // Paint structure
    RECT        rect;   // Rectangle structure

    switch (message)
    {
    case WM_CREATE:
    	{
            PlaySound(TEXT("hellowin.wav"), 0, SND_FILENAME | SND_ASYNC);
            return 0;
    	}
    case WM_PAINT:
        {
            hdc = BeginPaint(hwnd, &ps);
            GetClientRect(hwnd, &rect);
            DrawText(hdc, TEXT("Hello, Windows 98!"), -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
            EndPaint(hwnd, &ps);
            return 0;
        }
    case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
    default:
    	break;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}