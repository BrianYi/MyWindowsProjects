#include <windows.h>

#define BUFFER(y,x) *(pBuffer + y * cxBuffer + x)

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("Typer") ;
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

	hwnd = CreateWindow (szAppName, TEXT ("Typing Program"),
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
	static DWORD    dwCharSet   = DEFAULT_CHARSET;
	static int      cxChar, cyChar, cxClient, cyClient, cxBuffer, cyBuffer, xCaret, yCaret;
	static TCHAR    *pBuffer    = NULL;
	HDC             hdc;
	int             x, y, i;
	PAINTSTRUCT     ps;
	TEXTMETRIC      tm;

	switch (message) 
	{
	case WM_CREATE:
	case WM_INPUTLANGCHANGE:
	case WM_SIZE:
		cxClient	= LOWORD(lParam);
		cyClient	= HIWORD(lParam);
		
		hdc	= GetDC(hwnd);
		SelectObject(hdc, CreateFont(0, 0, 0, 0, 0, 0, 0, 0, dwCharSet, 0, 0, 0, FIXED_PITCH, NULL));
		GetTextMetrics(hdc, &tm);
		DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
		ReleaseDC(hwnd, hdc);
		cxChar	= tm.tmAveCharWidth;
		cyChar	= tm.tmHeight + tm.tmExternalLeading;
		cxBuffer	= max(1, cxClient / cxChar);
		cyBuffer	= max(1, cyClient / cyChar);
		xCaret	= 0;
		yCaret	= 0;
		
		if (pBuffer) {
			delete [] pBuffer;
		}

		pBuffer	= (TCHAR*)malloc(sizeof(TCHAR) * cxBuffer * cyBuffer);
		for (y = 0; y < cyBuffer; y++) {
			for (x = 0; x < cxBuffer; x++) {
				BUFFER(y,x)	= TCHAR(' ');
			}
		}

	case WM_SETFOCUS:
		CreateCaret(hwnd, NULL, cxChar, cyChar);
		SetCaretPos(xCaret * cxChar, yCaret * cyChar);
		ShowCaret(hwnd);
		return 0;

	case WM_KILLFOCUS:
		HideCaret(hwnd);
		DestroyCaret();
		return 0;

	case WM_KEYDOWN:
		switch (wParam) 
		{
		case VK_UP:
			yCaret	-= 1;
			yCaret	= max(0, yCaret);
			break;
		case VK_DOWN:
			yCaret	+= 1;
			yCaret	= min(yCaret, cyBuffer - 1);
			break;
		case VK_LEFT:
			xCaret	-= 1;
			xCaret	= max(0, xCaret);
			break;
		case VK_RIGHT:
			xCaret	+= 1;
			xCaret	= min(xCaret, cxBuffer - 1);
			break;
		case VK_HOME:
			xCaret	= 0;
			yCaret	= 0;
			break;
		case VK_DELETE:
			for (x = xCaret; x < cxBuffer - 1; x++) {
				BUFFER(yCaret, x)	= BUFFER(yCaret, x + 1);
			}
			BUFFER(yCaret, cxBuffer - 1)	= TCHAR(' ');
			hdc	= GetDC(hwnd);
			HideCaret(hwnd);
			SelectObject(hdc, CreateFont(0, 0, 0, 0, 0, 0, 0, 0, dwCharSet, 0, 0, 0, FIXED_PITCH, NULL));
			TextOut(hdc, xCaret * cxChar, yCaret * cyChar, &BUFFER(yCaret, xCaret), cxBuffer - xCaret);
			ShowCaret(hwnd);
			DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
			ReleaseDC(hwnd, hdc);
			break;
		default:
			break;
		}
		SetCaretPos(xCaret * cxChar, yCaret * cyChar);
		return 0;

	case WM_CHAR:
		switch (wParam) 
		{
		case '\b':
			if (xCaret > 0)
			{
				xCaret--;
				SendMessage(hwnd, WM_KEYDOWN, VK_DELETE, 1);
			}
			break;
		case '\t':
			do {
				SendMessage(hwnd, WM_CHAR, ' ', 1);
			} while (xCaret % 8);
			break;
		case '\n':
			if (++yCaret == cyBuffer) {
				yCaret = 0;
			}
			break;
		case '\r':
			xCaret	= 0;
			if (++yCaret == cyBuffer) {
				yCaret = 0;
			}
			break;
		case '\x1B':
			for (y = 0; y < cyBuffer; y++) {
				for (x = 0; x < cxBuffer; x++) {
					BUFFER(x, y) = ' ';
				}
			}
			xCaret  = 0;
			yCaret  = 0;

			InvalidateRect(hwnd, NULL, FALSE);
			break;
		default:
			BUFFER(yCaret, xCaret)	= wParam;
			HideCaret(hwnd);
			hdc	= GetDC(hwnd);
			SelectObject(hdc, CreateFont(0, 0, 0, 0, 0, 0, 0, 0, dwCharSet, 0, 0, 0, FIXED_PITCH, NULL));
			TextOut(hdc, xCaret * cxChar, yCaret * cyChar, &BUFFER(yCaret, xCaret), 1);

			if (++xCaret == cxBuffer) {
				xCaret	= 0;
				if (++yCaret == cyBuffer) {
					yCaret	= 0;
				}
			}
			DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
			ReleaseDC(hwnd, hdc);
			ShowCaret(hwnd);
			break;
		}
		SetCaretPos(xCaret * cxChar, yCaret * cyChar);
		return 0;

	case WM_PAINT:
		hdc	= BeginPaint(hwnd, &ps);
		HideCaret(hwnd);
		SelectObject(hdc, CreateFont(0, 0, 0, 0, 0, 0, 0, 0, dwCharSet, 0, 0, 0, FIXED_PITCH, NULL));
		for (y = 0; y < cyBuffer; y++) {
			for (x = 0; x < cxBuffer; x++) {
				TextOut(hdc, x * cxChar, y * cyChar, &BUFFER(y, x), 1);
			}
		}
		ShowCaret(hwnd);
		DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));		
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