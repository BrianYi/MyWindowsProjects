#include <Windows.h>
#include "SysMets.h"

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI WinMain ( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd )
{
	static TCHAR szAppName[] = TEXT ("SysMets1");
	HWND		hwnd ;
	MSG			msg ;
	WNDCLASS	wndclass ;

	wndclass.style			= CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc	= WndProc ;
	wndclass.cbClsExtra		= 0 ;
	wndclass.cbWndExtra		= 0 ;
	wndclass.hInstance		= hInstance ;
	wndclass.hIcon			= (HICON) LoadIcon (0, IDI_APPLICATION) ;
	wndclass.hCursor		= (HCURSOR) LoadCursor (0, IDC_ARROW) ;
	wndclass.hbrBackground	= (HBRUSH) GetStockObject(WHITE_BRUSH) ;
	wndclass.lpszMenuName	= 0 ;
	wndclass.lpszClassName	= szAppName ;

	if (!RegisterClass(&wndclass)) {
		MessageBox (0, TEXT("This program requires Windows NT!"),
			szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindow(szAppName, TEXT("Get System Metrics No. 1"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		0,
		0,
		hInstance,
		0);

	ShowWindow (hwnd, nShowCmd);
	UpdateWindow(hwnd);
	while (GetMessage(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int cxChar, cxCaps, cyChar ;
	PAINTSTRUCT	ps ;
	TCHAR		szBuffer [10] ;
	TEXTMETRIC	tm;

	switch (message) {
	case WM_CREATE:
		{
			HDC		hdc	= GetDC(hwnd);

			GetTextMetrics (hdc, &tm);
			cxChar = tm.tmAveCharWidth ;
			cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2 ;
			cyChar = tm.tmHeight + tm.tmExternalLeading ;

			ReleaseDC(hwnd, hdc);
			return 0;
		}
	case WM_PAINT:
		{
			HDC		hdc = BeginPaint(hwnd, &ps) ;

			for (int i = 0; i < NUMLINES ; i++) {
				TextOut(hdc, 0, cyChar * i, sysmetrics[i].szLabel,
					lstrlen(sysmetrics[i].szLabel));

				TextOut(hdc, 22 * cxCaps, cyChar * i, sysmetrics[i].szDesc, 
					lstrlen(sysmetrics[i].szDesc));

				SetTextAlign(hdc, TA_RIGHT | TA_TOP);
				
				TextOut(hdc, 22 * cxCaps + 40 * cxChar, cyChar * i, szBuffer,
					wsprintf(szBuffer, TEXT("%5d"), GetSystemMetrics(sysmetrics[i].iIndex)));

				SetTextAlign(hdc, TA_LEFT | TA_TOP);
			}
			EndPaint(hwnd, &ps);
			return 0;
		}
	case WM_DESTROY :
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
