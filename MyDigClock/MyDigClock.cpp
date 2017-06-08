#include <windows.h>

#define ID_TIMER    1

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("DigClock") ;
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

	hwnd = CreateWindow (szAppName, TEXT ("Digital Clock"),
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

void DrawNumber(HDC hdc, int number)
{
	BOOL fSevenSegment[10][7]	= {
		1, 1, 1, 0, 1, 1, 1,     // 0
		0, 0, 1, 0, 0, 1, 0,     // 1
		1, 0, 1, 1, 1, 0, 1,     // 2
		1, 0, 1, 1, 0, 1, 1,     // 3
		0, 1, 1, 1, 0, 1, 0,     // 4
		1, 1, 0, 1, 0, 1, 1,     // 5
		1, 1, 0, 1, 1, 1, 1,     // 6
		1, 0, 1, 0, 0, 1, 0,     // 7
		1, 1, 1, 1, 1, 1, 1,     // 8
		1, 1, 1, 1, 0, 1, 1		 // 9
	};

	POINT ptSegment[7][2]	= {
		10,10, 90,20,
		10,10, 20,95,
		80,10, 90,95,
		10,85, 90,95,
		10,85, 20,170,
		80,85, 90,170,
		10,160, 90,170
	};

	for (int i = 0; i < 7; i++) {
		if (fSevenSegment[number][i]) {
			Rectangle(hdc, ptSegment[i][0].x, ptSegment[i][0].y,
				ptSegment[i][1].x, ptSegment[i][1].y);
		}
	}
}

void DisplayTwoDigits(HDC hdc, int iNumber, BOOL fSuppress)
{
	if (fSuppress) {
		if (iNumber / 10) {
			DrawNumber(hdc, iNumber / 10);
		}
	} else {
		DrawNumber(hdc, iNumber / 10);		
	}
	OffsetWindowOrgEx(hdc, -100, 0, NULL);
	DrawNumber(hdc, iNumber % 10);
	OffsetWindowOrgEx(hdc, -100, 0, NULL);
}

void DisplayTime(HDC hdc, BOOL f24Hour, BOOL fSuppress)
{
	SYSTEMTIME	st;
	GetLocalTime(&st);
	
	if (f24Hour)
		DisplayTwoDigits(hdc, st.wHour, fSuppress);
	else
		DisplayTwoDigits(hdc, st.wHour % 12, fSuppress);
	DisplayTwoDigits(hdc, st.wMinute, fSuppress);
	DisplayTwoDigits(hdc, st.wSecond, fSuppress);
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HBRUSH	hBrush;
	PAINTSTRUCT		ps;
	HDC				hdc;
	static int		cxClient, cyClient;
	static BOOL		f24Hour, fSuppress;
	TCHAR			szBuffer[2];
	
	switch (message) 
	{
	case WM_CREATE:
		hBrush	= CreateSolidBrush(RGB(255,0,0));
		SetTimer(hwnd, ID_TIMER, 1000, NULL);
	case WM_SETTINGCHANGE:
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ITIME, szBuffer, 2);
		f24Hour	= szBuffer[0] == '1';
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ILZERO, szBuffer, 2);
		fSuppress = szBuffer[0] == '0';
		return 0;

	case WM_SIZE:
		cxClient	= LOWORD(lParam);
		cyClient	= HIWORD(lParam);
		return 0;

	case WM_TIMER:
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;

	case WM_PAINT:
		hdc	= BeginPaint(hwnd, &ps);
		SetMapMode(hdc, MM_ISOTROPIC);
		SetWindowExtEx(hdc, 600, 180, NULL);
		SetViewportExtEx(hdc, cxClient, cyClient, NULL);
		SetViewportOrgEx(hdc, cxClient / 2, cyClient / 2, NULL);
		SetWindowOrgEx(hdc, 600 / 2, 180 / 2, NULL);
		SelectObject(hdc, hBrush);
		SelectObject(hdc, GetStockObject(NULL_PEN));
		DisplayTime(hdc, f24Hour, fSuppress);
		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		KillTimer(hwnd, ID_TIMER);
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}