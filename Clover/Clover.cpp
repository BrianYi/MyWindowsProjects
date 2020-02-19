#include <windows.h>
#include <math.h>

#define TWO_PI (2.0 * 3.14159)

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("Clover") ;
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
		MessageBox (NULL, TEXT ("This program requires Windows NT!"),
			szAppName, MB_ICONERROR) ;
		return 0 ;
	}

	hwnd = CreateWindow (szAppName, TEXT ("Draw a Clover"),
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

LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HRGN	hRgnClip;
	static int	cxClient, cyClient;
	static int	cxWindowExt = 100, cyWindowExt = 100;
	double		fAngle, fRadius;
	HDC			hdc;
	HRGN		hRgnTemp[6];
	int			i;
	PAINTSTRUCT	ps;
	static POINT	aptData[8] = {
		-cxWindowExt / 2, -cyWindowExt / 6, 0, cyWindowExt / 6,
		0, cyWindowExt / 6, cxWindowExt / 2, -cyWindowExt / 6,
		-cxWindowExt / 6, 0, cxWindowExt / 6, cyWindowExt / 2,
		-cxWindowExt / 6, 0, cxWindowExt / 6, -cyWindowExt / 2
	};

    POINT   apt[8];

	switch (iMsg) 
	{
	case WM_SIZE:
		cxClient	= LOWORD(lParam);
		cyClient	= HIWORD(lParam);
		return 0;

	case WM_PAINT:
		hdc	= BeginPaint(hwnd, &ps);
		if (hRgnClip) 
		{
			DeleteObject(hRgnClip);
		}

		SetMapMode(hdc, MM_ANISOTROPIC);
		SetWindowExtEx(hdc, cxWindowExt, -cyWindowExt, NULL);
		SetViewportExtEx(hdc, cxClient, cyClient, NULL);
		// SetWindowOrgEx(hdc, -cxWindowExt / 2, cyWindowExt / 2, NULL);	//与下面的这句效果相同
		SetViewportOrgEx(hdc, cxClient / 2, cyClient / 2, NULL);
        for (i = 0; i < 8; i++) 
        {
            apt[i]  = aptData[i];
        }
		LPtoDP(hdc, apt, 8);

		for (i = 0; i < 4; i++) 
		{
			hRgnTemp[i]	= CreateEllipticRgn(apt[2 * i].x, apt[2 * i].y, apt[2 * i + 1].x, apt[2 * i + 1].y);
		}
		hRgnTemp[4]	= CreateRectRgn(0, 0, 1, 1);
		hRgnTemp[5]	= CreateRectRgn(0, 0, 1, 1);
		hRgnClip	= CreateRectRgn(0, 0, 1, 1);
		CombineRgn (hRgnTemp[4], hRgnTemp[0], hRgnTemp[1], RGN_OR) ;
		CombineRgn (hRgnTemp[5], hRgnTemp[2], hRgnTemp[3], RGN_OR) ;
		CombineRgn (hRgnClip,    hRgnTemp[4], hRgnTemp[5], RGN_XOR) ;

		for (i = 0; i < 6; i++) 
		{
			DeleteObject(hRgnTemp[i]);
		}
		SelectClipRgn(hdc, hRgnClip);

 		fRadius	= _hypot(cxWindowExt / 2.0, cyWindowExt / 2.0);
		for (fAngle = 0.0; fAngle < TWO_PI; fAngle += TWO_PI / 360) 
		{
			MoveToEx(hdc, 0, 0, NULL);
			// 0.5不明白有何用途
			LineTo(hdc, (int)(fRadius * cos(fAngle)/* + 0.5*/),
						(int)(fRadius * sin(fAngle)/* + 0.5*/));
		}

		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		DeleteObject(hRgnClip);
		PostQuitMessage(0);
		return 0;

	default:
		break;
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}