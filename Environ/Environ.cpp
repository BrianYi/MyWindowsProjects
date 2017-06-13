#include <windows.h>

#define ID_LIST     1
#define ID_TEXT     2

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("Environ") ;
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
	wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
	wndclass.lpszMenuName  = NULL ;
	wndclass.lpszClassName = szAppName ;

	if (!RegisterClass (&wndclass))
	{
		MessageBox (NULL, TEXT ("This program requires Windows NT!"),
			szAppName, MB_ICONERROR) ;
		return 0 ;
	}

	hwnd = CreateWindow (szAppName, TEXT ("Environment List Box"),
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

int FillListBox (HWND hwndList)
{
	int iLength, iMaxLength = 0;
	TCHAR *pVarBlock, *pVarBeg, *pVarEnd, *pVarName;

	pVarBlock = GetEnvironmentStrings();

	while (*pVarBlock) 
	{
		if (*pVarBlock != '=') 
		{
			pVarBeg = pVarBlock;
			while (*pVarBlock++ != '=') ;
			pVarEnd = pVarBlock - 1;
			iLength = pVarEnd - pVarBeg;
			iMaxLength = max(iMaxLength, iLength);

			pVarName = (TCHAR *)malloc((iLength + 1) * sizeof(TCHAR));
			CopyMemory(pVarName, pVarBeg, iLength * sizeof(TCHAR));
			pVarName[iLength] = '\0';

			SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)pVarName);
			free (pVarName);
		}
		while (*pVarBlock++);
	}
	return iMaxLength;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND  hwndList, hwndText ;
	int          iIndex, iLength, cxChar, cyChar ;
	TCHAR      * pVarName, * pVarValue ;
	static int   iMaxLength;
	switch (message) 
	{
	case WM_CREATE:
		hwndList = CreateWindow(TEXT("LISTBOX"), NULL,
						WS_CHILD | WS_VISIBLE | LBS_STANDARD,
						0, 0, 0, 0, 
						hwnd, (HMENU) ID_LIST,
						(HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
						NULL);

		hwndText = CreateWindow(TEXT("STATIC"), NULL,
						WS_CHILD | WS_VISIBLE | SS_LEFT,
						0, 0, 0, 0,
						hwnd, (HMENU) ID_TEXT,
						(HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
						NULL);

		iMaxLength = FillListBox(hwndList);
		return 0;

	case WM_SIZE:
		
		cxChar = LOWORD(GetDialogBaseUnits());
		cyChar = HIWORD(GetDialogBaseUnits());

		MoveWindow(hwndList, cxChar, cyChar * 3, 
			iMaxLength * cxChar + GetSystemMetrics(SM_CXVSCROLL), 
			cyChar * 6, TRUE);

		MoveWindow(hwndText, cxChar, cyChar,
			GetSystemMetrics(SM_CXSCREEN), cyChar, TRUE);
		return 0;

	case WM_SETFOCUS:
		SetFocus(hwndList);
		return 0;

	case WM_COMMAND:
		if (LOWORD(wParam) == ID_LIST && HIWORD(wParam) == LBN_SELCHANGE) 
		{
			iIndex = SendMessage(hwndList, LB_GETCURSEL, 0, 0);
			iLength = SendMessage(hwndList, LB_GETTEXTLEN, iIndex, 0) + 1;
			pVarName = (TCHAR *)malloc(iLength * sizeof(TCHAR));
			SendMessage(hwndList, LB_GETTEXT, iIndex, (LPARAM)pVarName);

			iLength = GetEnvironmentVariable(pVarName, NULL, 0);
			pVarValue = (TCHAR *)malloc(iLength * sizeof(TCHAR));
			GetEnvironmentVariable(pVarName, pVarValue, iLength);
			SetWindowText(hwndText, pVarValue);
			free (pVarName);
			free (pVarValue);
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}