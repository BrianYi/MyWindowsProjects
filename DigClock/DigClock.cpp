#include <windows.h>

#define ID_TIMER    1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("DigClock");
	HWND         hwnd;
	MSG          msg;
	WNDCLASS     wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("Program requires Windows NT!"),
			szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindow(szAppName, TEXT("Digital Clock"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

void DisplayDigit(HDC hdc, int iNumber)
{
	static BOOL		fSevenSegment[10][7] = {
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
	//*******************************
	//   _
	//	|_| => 7笔,构成LED显示结构
	//	|_|
	//
	//  00:00:00 => 6个位置
	static POINT ptSegment[7][6] = {
		7,   6,  11,  2,  31,  2,  35,  6,  31, 10,  11, 10,    //  _
		6,   7,  10, 11,  10, 31,   6, 35,   2, 31,   2, 11,    // |
		36,  7,  40, 11,  40, 31,  36, 35,  32, 31,  32, 11,    //   |
		7,  36,  11, 32,  31, 32,  35, 36,  31, 40,  11, 40,    //  _
		6,  37,  10, 41,  10, 61,   6, 65,   2, 61,   2, 41,    // |
		36, 37,  40, 41,  40, 61,  36, 65,  32, 61,  32, 41,    //   |
		7,  66,  11, 62,  31, 62,  35, 66,  31, 70,  11, 70     //  _
	};
	int iSeg;
	// 绘制iNumber
	for (iSeg = 0; iSeg < 7; iSeg++) {
		if (fSevenSegment[iNumber][iSeg]) {
			Polygon(hdc, ptSegment[iSeg], 6);
		}
	}
}

void DisplayTwoDigits(HDC hdc, int iNumber, BOOL fSuppress)
{
	if (!fSuppress || (iNumber / 10 != 0)) {
		DisplayDigit(hdc, iNumber / 10);
	}

	OffsetWindowOrgEx(hdc, -42, 0, NULL);	// 逻辑坐标系左移(视口原点位置不变),那么原来的逻辑坐标横轴都变大了
	DisplayDigit(hdc, iNumber % 10);
	OffsetWindowOrgEx(hdc, -42, 0, NULL);	// 逻辑坐标系左移(视口原点位置不变),那么原来的逻辑坐标横轴都变大了
}

void DisplayColon(HDC hdc)
{
	POINT ptColon[2][4] = {
		2,  21,  6,  17,  10, 21,  6, 25,
		2,  51,  6,  47,  10, 51,  6, 55
	};

	Polygon(hdc, ptColon[0], 4);
	Polygon(hdc, ptColon[1], 4);

	OffsetWindowOrgEx(hdc, -12, 0, NULL);
}

void DisplayTime(HDC hdc, BOOL f24Hour, BOOL fSuppress)
{
	SYSTEMTIME	st;

	GetLocalTime(&st);	// 获取本地时间
	// 绘制小时
	if (f24Hour) {
		DisplayTwoDigits(hdc, st.wHour, fSuppress);
	}
	else {
		DisplayTwoDigits(hdc, (st.wHour %= 12) ? st.wHour : 12, fSuppress);
	}

	DisplayColon(hdc);	// 绘制冒号
	DisplayTwoDigits(hdc, st.wMinute, FALSE);	// 绘制分
	DisplayColon(hdc);	// 绘制冒号
	DisplayTwoDigits(hdc, st.wSecond, FALSE);	// 绘制秒
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BOOL		f24Hour, fSuppress;
	static HBRUSH	hBrushRed;
	static int		cxClient, cyClient;
	HDC				hdc;
	PAINTSTRUCT		ps;
	TCHAR			szBuffer[64];
	POINT			ptScreenPos, ptClientPos, ptLogicPos;

	switch (message)
	{
	case WM_CREATE:
		hBrushRed = CreateSolidBrush(RGB(255, 0, 0));
		SetTimer(hwnd, ID_TIMER, 1000, NULL);	// 设置计时器,每秒(不精确,也可能是989毫秒)Windows向窗口过程发送一次WM_TIMER
	case WM_SETTINGCHANGE:
		//*******************************
		// 是否为24小时制
		//	0: AM/PM 12-hour format
		//	1: 24-hour format
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ITIME, szBuffer, 2);
		f24Hour = (szBuffer[0] == '1');

		//*******************************
		// 是否时间格式含有前导0
		//	0: No leading zeros for hours
		//	1: Leading zeros for hours
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ITLZERO, szBuffer, 2);
		fSuppress = (szBuffer[0] == '0');

		InvalidateRect(hwnd, NULL, TRUE);
		return 0;
	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		return 0;
	case WM_MOUSEMOVE:
	case WM_TIMER:
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		SaveDC(hdc);
		SetMapMode(hdc, MM_ISOTROPIC);	// 长宽尺度不变,这三行代码顺序不变
		SetWindowExtEx(hdc, 276, 72, NULL);	
		SetViewportExtEx(hdc, cxClient, cyClient, NULL);

		SetWindowOrgEx(hdc, 276 / 2, 72 / 2, NULL);
		SetViewportOrgEx(hdc, cxClient / 2, cyClient / 2, NULL);
		GetCursorPos(&ptScreenPos);
		ptLogicPos = ptClientPos = ptScreenPos;
		ScreenToClient(hwnd, &ptClientPos);
		DPtoLP(hdc, &ptLogicPos, 1);
		SelectObject(hdc, GetStockObject(NULL_PEN));
		SelectObject(hdc, hBrushRed);

		DisplayTime(hdc, f24Hour, fSuppress);

		RestoreDC(hdc, -1);
		TextOut(hdc, 10, 10, szBuffer, wsprintf(szBuffer, TEXT("屏幕坐标(%3d, %3d)"), ptScreenPos.x, ptScreenPos.y));
		TextOut(hdc, 10, 40, szBuffer, wsprintf(szBuffer, TEXT("窗口客户区坐标(%3d, %3d)"), ptClientPos.x, ptClientPos.y));
		TextOut(hdc, 10, 70, szBuffer, wsprintf(szBuffer, TEXT("逻辑坐标(%3d, %3d)"), ptLogicPos.x, ptLogicPos.y));

		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		KillTimer(hwnd, ID_TIMER);
		DeleteObject(hBrushRed);
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}