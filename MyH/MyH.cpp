#include <windows.h>

#define ID_LIST 1
#define ID_TEXT 2
#define ID_READER 3

//#define MAXREAD 8192
#define DIRATTR		(DDL_READWRITE | DDL_READONLY | DDL_HIDDEN | DDL_SYSTEM | \
	DDL_DIRECTORY | DDL_ARCHIVE | DDL_DRIVES)
#define DTFLAGS		(DT_WORDBREAK | DT_EXPANDTABS | DT_NOCLIP | DT_NOPREFIX)

LRESULT CALLBACK WndProc  (HWND, UINT, WPARAM, LPARAM) ;
LRESULT CALLBACK ListProc (HWND, UINT, WPARAM, LPARAM) ;
LRESULT CALLBACK ReaderProc (HWND, UINT, WPARAM, LPARAM) ;

WNDPROC OldList ;
WNDPROC OldReader ;
/*static RECT     rect ;*/
/*static */BYTE		*pbBuffer = NULL;
/*static */int		iNumOfBytesRead;
/*static */BOOL     bValidFile ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("head") ;
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
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH) ;
	wndclass.lpszMenuName  = NULL ;
	wndclass.lpszClassName = szAppName ;

	if (!RegisterClass (&wndclass))
	{
		MessageBox (NULL, TEXT ("This program requires Windows NT!"),
			szAppName, MB_ICONERROR) ;
		return 0 ;
	}

	hwnd = CreateWindow (szAppName, TEXT ("head"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
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

LRESULT CALLBACK ListProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return CallWindowProc(OldList, hwnd, message, wParam, lParam);
}

void SplitBytes(__out BYTE **&ppbByteArry, __out LPDWORD lpdwNumberOfArry, __out DWORD **ppdwByteArrySize, 
	__in BYTE *pbBytes, __in DWORD dwNumberOfByteSize, __in BYTE bSepartor)
{
	BYTE *p = pbBytes;
	DWORD dwNumberOfSeparator = 0;
	DWORD i, j, dwByteBeg = 0, dwByteEnd = 0, dwByteLength = 0;
	for (i = 0; i < dwNumberOfByteSize; i++) 
	{
		if (pbBytes[i] == bSepartor)
			dwNumberOfSeparator++;
	}
	dwNumberOfSeparator++;
	*lpdwNumberOfArry = dwNumberOfSeparator;
	ppbByteArry = (BYTE**)malloc(sizeof(BYTE*) * dwNumberOfSeparator);
	*ppdwByteArrySize = (DWORD*)malloc(sizeof(DWORD) * dwNumberOfSeparator);
	for (i = 0, j = 0; i < dwNumberOfByteSize && j < dwNumberOfSeparator; i++) 
	{
		if (pbBytes[i] == bSepartor) 
		{
			dwByteEnd = i;
			dwByteLength = dwByteEnd - dwByteBeg;
			(*ppdwByteArrySize)[j] = dwByteLength;
			ppbByteArry[j] = (BYTE*)malloc(sizeof(BYTE) * dwByteLength);
			CopyMemory(ppbByteArry[j], pbBytes + dwByteBeg, dwByteLength);
			j++;
			dwByteBeg = i+1;
		}
	}
	if (dwByteBeg < dwNumberOfByteSize) 
	{
		dwByteLength = dwNumberOfByteSize - dwByteBeg;
		(*ppdwByteArrySize)[j] = dwByteLength;
		ppbByteArry[j] = (BYTE*)malloc(sizeof(BYTE) * dwByteLength);
		CopyMemory(ppbByteArry[j], pbBytes + dwByteBeg, dwByteLength);
	}
}

LRESULT CALLBACK ReaderProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	HDC             hdc ;
	PAINTSTRUCT     ps ;
	RECT			rect;
	static int		i, cxChar, cyChar, iBegPaint, 
		iEndPaint, cxClient, cyClient, cyLine, iVerPos;
	SCROLLINFO	si ;
	switch (message) 
	{
	case WM_SIZE:
		cxChar = LOWORD(GetDialogBaseUnits());
		cyChar = HIWORD(GetDialogBaseUnits());
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		cyLine = (cyClient - GetSystemMetrics(SM_CYHSCROLL)) / cyChar;
// 		rect.left = cxChar;
// 		rect.right = LOWORD(lParam) - cxChar;
// 		rect.top = 0;
// 		rect.bottom = rect.top + 5 * cyChar;
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		if (bValidFile)
		{
			DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT)));
            //SetBkMode(hdc, OPAQUE);
			SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
			SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));
			BYTE **pbByteArry;
			DWORD dwNumberOfArry;
			DWORD *pdwByteArrySize;
			SplitBytes(pbByteArry, &dwNumberOfArry, &pdwByteArrySize, pbBuffer, iNumOfBytesRead, BYTE('\n'));
			//DrawTextA(hdc, (LPCSTR)pbBuffer, iNumOfBytesRead, &rect, DTFLAGS);
			iBegPaint = max(0, iVerPos + ps.rcPaint.top / cyChar);
			iEndPaint = min(iVerPos + ps.rcPaint.bottom / cyChar, cyLine);
			for (i = iBegPaint; i <= iEndPaint /*cyLine && i < dwNumberOfArry*/; i++) 
			{
				rect.left = 0;
				rect.right = cxClient;
				rect.top = i * cyChar;
				rect.bottom = rect.top + cyChar;

				//TextOutA(hdc, 0, cyChar * i, , pdwByteArrySize[i]);
				DrawTextA(hdc, (LPCSTR)pbByteArry[i], pdwByteArrySize[i], &rect, DTFLAGS);
			}
			for (i = 0; i < dwNumberOfArry; i++) 
			{
				free (pbByteArry[i]);
				pbByteArry[i] = NULL;
			}
			free (pdwByteArrySize);
			free (pbByteArry);
		}
		EndPaint(hwnd, &ps);
		return 0;
	}
	return CallWindowProc(OldReader, hwnd, message, wParam, lParam);
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	/*static BOOL     bValidFile ;*/
	//static BYTE     buffer[MAXREAD] ;
	static HWND     hwndList, hwndText, hwndReader;
	static RECT     rect ;
	static TCHAR    szFile[MAX_PATH + 1] ;
	/*static int		iNumOfBytesRead;*/
	static DWORD	dwFileSize;
	/*static BYTE		*pbBuffer = NULL;*/
	HANDLE          hFile ;
	HDC             hdc ;
	int             i, cxChar, cyChar ;
	PAINTSTRUCT     ps ;
	TCHAR           szBuffer[MAX_PATH + 1] ;

	switch (message) 
	{
	case WM_CREATE:
		hwndList = CreateWindow(TEXT("LISTBOX"), NULL,
						WS_CHILD | WS_VISIBLE | LBS_STANDARD,
						0, 0, 0, 0,
						hwnd, (HMENU) ID_LIST,
						(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

		hwndText = CreateWindow(TEXT("STATIC"), NULL,
						WS_CHILD | WS_VISIBLE | SS_LEFT,
						0, 0, 0, 0,
						hwnd, (HMENU) ID_TEXT,
						(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

		hwndReader = CreateWindow(TEXT("STATIC"), NULL,
						WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_BORDER |
						SS_LEFT,
						0, 0, 0, 0,
						hwnd, (HMENU) ID_READER,
						((LPCREATESTRUCT)lParam)->hInstance, NULL);

		OldList = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG)ListProc);
		OldReader = (WNDPROC)SetWindowLongPtr(hwndReader, GWLP_WNDPROC, (LONG)ReaderProc);
		
		SendMessage(hwndList, LB_DIR, DIRATTR, (LPARAM)TEXT("*.*"));
		return 0;

	case WM_SIZE:

		cxChar = LOWORD(GetDialogBaseUnits());
		cyChar = HIWORD(GetDialogBaseUnits());

		MoveWindow(hwndText, cxChar, cyChar, LOWORD(lParam), cyChar, TRUE);
		MoveWindow(hwndList, cxChar, cyChar * 3, 
			13 * cxChar + GetSystemMetrics(SM_CXVSCROLL), 
			cyChar * 6, TRUE);

		rect.left = 3 * cxChar + 13 * cxChar + GetSystemMetrics(SM_CXVSCROLL);
		rect.top = 3 * cyChar;
		rect.right = LOWORD(lParam) - rect.left - GetSystemMetrics(SM_CXVSCROLL);
		rect.bottom = HIWORD(lParam) - rect.top - GetSystemMetrics(SM_CYHSCROLL);
		MoveWindow(hwndReader, rect.left, rect.top, rect.right, rect.bottom, TRUE);

		return 0;

// 	case WM_SETFOCUS:
// 		SetFocus(hwndList);
// 		return 0;

	case WM_COMMAND:
		if (LOWORD(wParam) == ID_LIST && HIWORD(wParam) == LBN_DBLCLK) 
		{
			if (LB_ERR == (i = SendMessage(hwndList, LB_GETCURSEL, 0, 0)))
				break;
			SendMessage(hwndList, LB_GETTEXT, i, (LPARAM)szBuffer);
			if (INVALID_HANDLE_VALUE != (hFile = CreateFile(szBuffer, GENERIC_READ, FILE_SHARE_READ, NULL,
							OPEN_EXISTING, 0, NULL))) 
			{
				// 成功打开文件
				bValidFile = TRUE;
				lstrcpy(szFile, szBuffer);
				GetCurrentDirectory(MAX_PATH+1, szBuffer);
				if (szBuffer[lstrlen(szBuffer)-1] != '\\') 
					lstrcat(szBuffer, TEXT("\\"));
				SetWindowText(hwndText, lstrcat(szBuffer,szFile));
				if (INVALID_FILE_SIZE == (dwFileSize = GetFileSize(hFile, NULL)))
					break;
				pbBuffer ? free(pbBuffer):0;
				pbBuffer = (BYTE*)malloc(dwFileSize * sizeof(BYTE));
				ReadFile(hFile, pbBuffer, dwFileSize, (LPDWORD)&iNumOfBytesRead, NULL);
				CloseHandle(hFile);
				InvalidateRect(hwndReader, NULL, TRUE);
				//SetWindowTextA(hwndEdit, (LPCSTR)buffer);
			}
			else 
			{
				// 未打开文件，可能是文件夹或磁盘driver
				bValidFile = FALSE;
				szBuffer[lstrlen(szBuffer)-1] = TEXT('\0');

				// 将线程当前目录设为
				if (!SetCurrentDirectory(szBuffer + 1)) 
				{
					// 说明为磁盘driver
					szBuffer[3] = TEXT(':');
					szBuffer[4] = TEXT('\0');
					SetCurrentDirectory(szBuffer + 2);
				}
				GetCurrentDirectory(MAX_PATH+1, szBuffer);
				SetWindowText(hwndText, szBuffer);

				// 重新显示当前目录
				SendMessage(hwndList, LB_RESETCONTENT, 0, 0);
				SendMessage(hwndList, LB_DIR, DIRATTR, (LPARAM)TEXT("*.*"));
				
			}
			InvalidateRect (hwnd, NULL, TRUE) ;
		}
		return 0;

// 	case WM_PAINT:
// 		hdc = BeginPaint(hwnd, &ps);
// 		if (bValidFile)
// 		{
// 			DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT)));
// 			SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
// 			SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));
// 			DrawTextA(hdc, (LPCSTR)pbBuffer, iNumOfBytesRead, &rect, DTFLAGS);
// 		}
// 		EndPaint(hwnd, &ps);
// 		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}