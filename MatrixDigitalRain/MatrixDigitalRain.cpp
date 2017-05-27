#include <windows.h>

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("MatrixDigitalRain") ;
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
	wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH) ;
	wndclass.lpszMenuName  = NULL ;
	wndclass.lpszClassName = szAppName ;

	if (!RegisterClass (&wndclass))
	{
		MessageBox (NULL, TEXT ("This program requires Windows NT!"), 
			szAppName, MB_ICONERROR) ;
		return 0 ;
	}

	hwnd = CreateWindow (szAppName, TEXT ("Keyboard Message Viewer #1"),
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

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	PAINTSTRUCT	ps;
	TEXTMETRIC	tm;
	static int	cxClient, cyClient, cxChar, cyChar, cxCaps;
	static TCHAR szChar[] = TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	static TCHAR szMatrix[256];
	static int	cMatrixSize = sizeof(szMatrix) / sizeof(szMatrix[0]);
	
	switch (message) 
	{
	case WM_CREATE:
		
		return 0;

	case WM_SIZE:
		cxClient	= LOWORD(lParam);
		cyClient	= HIWORD(lParam);
		return 0;

	case WM_KEYDOWN:
		if (wParam == VK_DOWN) 
		{
			ScrollWindow(hwnd, 0, 20, NULL, NULL);
		}
		else if (wParam == VK_UP)
		{
			ScrollWindow(hwnd, 0, -20, NULL, NULL);
		}
		
		return 0;

	case WM_PAINT:
		hdc	= BeginPaint(hwnd, &ps);
		SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
		SetBkMode(hdc, TRANSPARENT);
		for (int i = 1; i <= 30; i++) 
		{
			for (int j = 0; j < cMatrixSize; j++) 
			{
				szMatrix[j]	= szChar[rand() % 26];
			}
			if (i == 30) 
			{
				SetTextColor(hdc, RGB(255, 255, 255));
				TextOut(hdc, 10, 10 + (i-1) * 20, szMatrix, cMatrixSize);
			}
			else if (i <= 15)
			{
				SetTextColor(hdc, RGB(0, 255 / 15 * i, 0));
				TextOut(hdc, 10, 10 + (i-1) * 20, szMatrix, cMatrixSize);
			}
			else if (i <= 29)
			{
				SetTextColor(hdc, RGB(255 / 15 * (i - 15), 255, 255 / 15 * (i - 15)));
				TextOut(hdc, 10, 10 + (i-1) * 20, szMatrix, cMatrixSize);
			}
		}
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