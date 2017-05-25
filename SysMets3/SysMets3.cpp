#define WINVER 0x0500
#include <windows.h>
#include "sysmets.h"

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("SysMets3") ;
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

	hwnd = CreateWindow (szAppName, TEXT ("Get System Metrics No. 3"),
		WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
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
	static int cxChar, cxCaps, cyChar, cxClient, cyClient, iMaxWidth ;
	HDC			hdc ;
	int			i, x, y, iVertPos, iHorzPos, iPaintBeg, iPaintEnd ;
	PAINTSTRUCT	ps ;
	SCROLLINFO	si ;
	TCHAR		szBuffer[10] ;
	TEXTMETRIC	tm ;

	switch (message) 
	{
	case WM_CREATE:

		hdc = GetDC(hwnd);

		// calculate font width and height
		GetTextMetrics(hdc, &tm);
		cxChar = tm.tmAveCharWidth ;
		cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2 ;
		cyChar = tm.tmHeight + tm.tmExternalLeading ;

		ReleaseDC(hwnd, hdc);

		iMaxWidth = 40 * cxChar + 22 * cxCaps ;
		return 0;

	case WM_SIZE:

		// get new client area size
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);

		// reset vertical scroll info
		si.cbSize	= sizeof (si);
		si.fMask	= SIF_RANGE | SIF_PAGE ;
		si.nMin		= 0 ;
		si.nMax		= NUMLINES - 1 ;
		si.nPage	= cyClient / cyChar ;
		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

		// reset horizontal scroll info
		si.cbSize	= sizeof (si);
		si.fMask	= SIF_RANGE | SIF_PAGE ;
		si.nMin		= 0;
		si.nMax		= 2 + iMaxWidth / cxChar;
		si.nPage	= cxClient / cxChar ;
		SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
		return 0;

	case WM_VSCROLL:

		// get old scroll info
		si.cbSize	= sizeof (si);
		si.fMask	= SIF_ALL ;
		GetScrollInfo(hwnd, SB_VERT, &si);

		// save old scrollbar position
		iVertPos	= si.nPos ;

		// calculate new scrollbar position
		switch (LOWORD(wParam)) 
		{
		case SB_TOP:
			si.nPos	= si.nMin ;
			break;

		case SB_BOTTOM:
			si.nPos	= si.nMax ;
			break;

		case SB_LINEUP:
			si.nPos -= 1 ;
			break;

		case SB_LINEDOWN:
			si.nPos += 1 ;
			break;

		case SB_PAGEUP:
			si.nPos -= si.nPage;
			break;

		case SB_PAGEDOWN:
			si.nPos += si.nPage ;
			break;

		case SB_THUMBTRACK:
			si.nPos = si.nTrackPos ;
			break;

		default:
			break;
		}

		// set vertical scroll bar with new position
		si.fMask	= SIF_POS ;
		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);	// repaint vertical scroll bar
		GetScrollInfo(hwnd, SB_VERT, &si);

		// if new pos is not equal to old pos, then send the WM_PAINT message to window procedure
		if (si.nPos != iVertPos) 
		{
			ScrollWindow(hwnd, 0, cyChar * (iVertPos - si.nPos), 0, 0);
			UpdateWindow(hwnd);
		}
		return 0;

	case WM_HSCROLL:
		si.cbSize	= sizeof (si);
		si.fMask	= SIF_ALL;
		GetScrollInfo(hwnd, SB_HORZ, &si);

		iHorzPos = si.nPos ;

		switch (LOWORD(wParam)) 
		{
		case SB_LINELEFT:
			si.nPos -= 1 ;
			break ;

		case SB_LINERIGHT:
			si.nPos += 1 ;
			break;

		case SB_PAGELEFT:
			si.nPos -= si.nPage ;
			break;

		case SB_PAGERIGHT:
			si.nPos += si.nPage ;
			break;

		case SB_THUMBTRACK:
			si.nPos = si.nTrackPos;

		default:
			break;
		}

		si.fMask	= SIF_POS;
		SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
		GetScrollInfo(hwnd, SB_HORZ, &si);

		if (si.nPos != iHorzPos) 
		{
			ScrollWindow(hwnd, cxChar * (iHorzPos - si.nPos), 0, 0, 0);
			UpdateWindow(hwnd);
		}
		return 0;

	case WM_PAINT:
		hdc	= BeginPaint(hwnd, &ps);	// validate region of ps.rcPaint

		// set mask for getting data from scroll info
		si.cbSize	= sizeof (si);
		si.fMask	= SIF_POS ;

		// get old position from vertical scroll bar
		GetScrollInfo(hwnd, SB_VERT, &si);
		iVertPos	= si.nPos ;

		// get old position from horizontal scroll bar
		GetScrollInfo(hwnd, SB_HORZ, &si);
		iHorzPos	= si.nPos ;

		// set window origin
		SetWindowOrgEx(hdc, iHorzPos * cxChar, iVertPos * cyChar, 0);

		// calculate absolute position of uncovered invalidated region in the whole full page
		iPaintBeg	= max (0, iVertPos + ps.rcPaint.top / cyChar) ;
		iPaintEnd	= min (NUMLINES - 1, iVertPos + ps.rcPaint.bottom / cyChar) ;

		for (i = iPaintBeg; i <= iPaintEnd ; i++) 
		{
			x	= cxChar * (1/* - iHorzPos*/) ;
			y	= cyChar * (i/* - iVertPos*/) ;

			TextOut(hdc, x, y, sysmetrics[i].szLabel,
				lstrlen(sysmetrics[i].szLabel));

			TextOut(hdc, x + 22 * cxCaps, y, sysmetrics[i].szDesc,
				lstrlen(sysmetrics[i].szDesc));

			SetTextAlign(hdc, TA_RIGHT | TA_TOP);

			TextOut(hdc, x + 22 * cxCaps + 40 * cxChar, y, szBuffer,
				wsprintf(szBuffer, TEXT("%5d"),
				GetSystemMetrics(sysmetrics[i].iIndex))) ;

			SetTextAlign(hdc, TA_LEFT | TA_TOP);
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