#include <windows.h>

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("AltWind") ;
	HWND         hwnd ;
	MSG          msg ;
	WNDCLASS     wndclass ;

	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = WndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInstance ;
	wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
	wndclass.lpszMenuName  = NULL ;
	wndclass.lpszClassName = szAppName ;

	if (!RegisterClass (&wndclass))
	{
		MessageBox (NULL, TEXT ("Program requires Windows NT!"), 
			szAppName, MB_ICONERROR) ;
		return 0 ;
	}

	hwnd = CreateWindow (szAppName, TEXT ("Alternate and Winding Fill Modes"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL) ;

	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ;

	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}
	return msg.wParam ;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT	ps;
	static int	cxClient, cyClient;
	HDC			hdc;
	POINT		polygonData[11]	= {
		1,1, 1,3, 3,3, 3,1, 4,1, 
		4,6, 2,6, 2,4, 5,4, 5,5, 3,5
	};
	POINT		apt[11];
	switch (message) 
	{
	case WM_SIZE:
		cxClient	= LOWORD(lParam);
		cyClient	= HIWORD(lParam);
		return 0;

	case WM_PAINT:
		hdc	= BeginPaint(hwnd, &ps);
		SelectObject(hdc, GetStockObject(GRAY_BRUSH));
		SetMapMode(hdc, MM_ANISOTROPIC);
		SetWindowExtEx(hdc, 12, -12, NULL);
		SetViewportExtEx(hdc, cxClient, cyClient, NULL);
		SetWindowOrgEx(hdc, 0, 12, NULL);

		for (int i = 0; i < 11; i++) 
		{
			apt[i]	= polygonData[i];
		}
		SetPolyFillMode(hdc, ALTERNATE);
		Polygon(hdc, apt, 11);

		for (int i = 0; i < 11; i++) 
		{
			apt[i]		= polygonData[i];
			apt[i].x	+= 5;
		}
		SetPolyFillMode(hdc, WINDING);
		Polygon(hdc, apt, 11);

		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}