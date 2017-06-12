#include <windows.h>

LRESULT CALLBACK WndProc    (HWND, UINT, WPARAM, LPARAM) ;
LRESULT CALLBACK ScrollProc (HWND, UINT, WPARAM, LPARAM) ;

int     idFocus ;
WNDPROC OldScroll[3] ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("Colors1") ;
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
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH) ;
	wndclass.lpszMenuName  = NULL ;
	wndclass.lpszClassName = szAppName ;

	if (!RegisterClass (&wndclass))
	{
		MessageBox (NULL, TEXT ("This program requires Windows NT!"),
			szAppName, MB_ICONERROR) ;
		return 0 ;
	}

	hwnd = CreateWindow (szAppName, TEXT ("Color Scroll"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL) ;

	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ;

	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage  (&msg) ;
	}
	return msg.wParam ;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static COLORREF crPrim[3]   = { RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255) };
	static HBRUSH   hBrush[3];
	static HWND     hwndScroll[3], hwndLabel[3], hwndValue[3], hwndRect;
	static int      color[3], cyChar;
	static RECT     rcColor;
	static TCHAR    *szColorLable[] = { TEXT("Red"),    TEXT("Green"),  TEXT("Blue") };
	HINSTANCE       hInstance;
	int             i, cxClient, cyClient;
	TCHAR           szBuffer[10];

	switch (message) 
	{
	case WM_CREATE:
		hInstance = ((LPCREATESTRUCT)lParam)->hInstance;

		cyChar = HIWORD(GetDialogBaseUnits());
		
		//*******************************
		// 不明白为啥需要先创建这个控件，若放在其他controls后面创建，就会先显示该控件
		hwndRect = CreateWindow(TEXT("STATIC"), NULL, WS_VISIBLE | WS_CHILD | SS_WHITERECT,
			0, 0, 0, 0, hwnd, (HMENU)9, hInstance, NULL);

		for (i = 0; i < 3; i++) 
		{
			// 0~2 scrollbar
			hwndScroll[i] = CreateWindow(TEXT("SCROLLBAR"), NULL, WS_VISIBLE | WS_CHILD | SBS_VERT | WS_TABSTOP,
									0, 0, 0, 0, hwnd, (HMENU)i, hInstance, NULL);
			SetScrollRange(hwndScroll[i], SB_CTL, 0, 255, TRUE);
			SetScrollPos(hwndScroll[i], SB_CTL, 0, TRUE);

			// 3~5 static text
			hwndLabel[i] = CreateWindow(TEXT("STATIC"), szColorLable[i], WS_VISIBLE | WS_CHILD | SS_CENTER,
									0, 0, 0, 0, hwnd, (HMENU)(i + 3), hInstance, NULL);

			// 6~8 static text
			hwndValue[i] = CreateWindow(TEXT("STATIC"), TEXT("0"), WS_VISIBLE | WS_CHILD | SS_CENTER,
									0, 0, 0, 0, hwnd, (HMENU)(i + 6), hInstance, NULL);

			hBrush[i] = CreateSolidBrush(crPrim[i]);

            //*******************************
            // 设置新的WNDPPROC，存储旧的WNDPROC到OldScroll
			OldScroll[i] = (WNDPROC)SetWindowLongPtr(hwndScroll[i], GWLP_WNDPROC, (LONG)ScrollProc);
		}
		return 0;

	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);

		// set color rect
		SetRect(&rcColor, cxClient / 2, 0, cxClient, cyClient);

		// reposition window control
		MoveWindow(hwndRect, 0, 0, cxClient / 2, cyClient, TRUE);

		for (i = 0; i < 3; i++)
		{
			MoveWindow(hwndLabel[i], (2 * i + 1) * cxClient / 14, cyChar, cxClient / 14, cyChar, TRUE);
			MoveWindow(hwndScroll[i], (2 * i + 1) * cxClient / 14, 2 * cyChar, cxClient / 14, cyClient - 4 * cyChar, TRUE);
			MoveWindow(hwndValue[i], (2 * i + 1) * cxClient / 14, cyClient - 2 * cyChar, cxClient / 14, cyChar, TRUE);
		}

		// 
		SetFocus(hwnd);

		return 0;

	case WM_CTLCOLORSTATIC:
		//i = GetWindowLongPtr((HWND)lParam, GWLP_ID);
        i = GetDlgCtrlID((HWND)lParam);
		if (i >= 3 && i <= 8)
		{
			SetTextColor((HDC)wParam, crPrim[i % 3]);
			SetBkColor((HDC)wParam, GetSysColor(COLOR_BTNHIGHLIGHT));
			return (LRESULT)GetStockObject(WHITE_BRUSH);
		}
		break;

	case WM_CTLCOLORSCROLLBAR:
		i = GetDlgCtrlID((HWND)lParam);
		if (i >= 0 && i <= 2) 
		{
			return (LRESULT)hBrush[i];
		}
		break;

	case WM_VSCROLL:
		/*i = GetWindowLongPtr((HWND)lParam, GWLP_ID);*/
        i = GetDlgCtrlID((HWND)lParam);
		switch (LOWORD(wParam)) 
		{
		case SB_LINEDOWN:
			color[i]++;
			break;
		case SB_LINEUP:
			color[i]--;
			break;
		case SB_PAGEDOWN:
			color[i] += 16;
			break;
		case SB_PAGEUP:
			color[i] -= 16;
			break;
		case SB_BOTTOM:
			color[i] = 255;
			break;
		case SB_TOP:
			color[i] = 0;
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			color[i] = HIWORD(wParam);
			break;
		}
		color[i] = max(0, min(color[i], 255));
		SetScrollPos(hwndScroll[i], SB_CTL, color[i], TRUE);
		wsprintf(szBuffer, TEXT("%d"), color[i]);
		SetWindowText(hwndValue[i], szBuffer);
		DeleteObject((HBRUSH)SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG)CreateSolidBrush(RGB(color[0], color[1], color[2]))));
		SetFocus(hwndScroll[i]);
		idFocus = i;
		InvalidateRect(hwnd, &rcColor, TRUE);
		return 0;

	case WM_SETFOCUS:
		SetFocus(hwndScroll[idFocus]);
		return 0;

	case WM_DESTROY:
		DeleteObject((HBRUSH)SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG)GetStockObject(WHITE_BRUSH)));
		for (i = 0; i < 3; i++) 
		{
			DeleteObject(hBrush[i]);
		}
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK ScrollProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//int id = GetWindowLongPtr(hwnd, GWLP_ID);
    int id = GetDlgCtrlID(hwnd);

	switch (message) 
	{
	case WM_KEYDOWN:
		if (wParam == VK_TAB) 
		{
			SetFocus(GetDlgItem(GetParent(hwnd), (id + (GetKeyState(VK_SHIFT) < 0 ? (3-1) : 1)) % 3));
		}
		break;
	case WM_SETFOCUS:
		id = idFocus;
		break;
	}
	return CallWindowProc(OldScroll[id], hwnd, message, wParam, lParam);
}