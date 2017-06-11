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
    wndclass.hbrBackground = CreateSolidBrush (0) ;
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
    static HBRUSH   hBrush[3], hBrushStatic;
    static HWND     hwndScroll[3], hwndLabel[3], hwndValue[3], hwndRect;
    static int      color[3], cyChar;
    static RECT     rcColor;
    static TCHAR    *szColorLable[] = { TEXT("Red"),    TEXT("Green"),  TEXT("Blue") };
    HINSTANCE       hInstance;
    int             i, cxClient, cyClient;
    TCHAR           szBuffer[10];
    
    switch (message) {
    case WM_CREATE:
        hInstance   = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);

        hwndRect    = CreateWindow(TEXT("static"), NULL,
                                   WS_CHILD | WS_VISIBLE | SS_WHITERECT,
                                   0, 0, 0, 0,
                                   hwnd, (HMENU) 9, hInstance, NULL);

        for (i = 0; i < 3; i++) {

            hwndScroll[i]   = CreateWindow(TEXT("scrollbar"), NULL,
                                           WS_CHILD | WS_VISIBLE |
                                           WS_TABSTOP | SBS_VERT,
                                           0, 0, 0, 0,
                                           hwnd, (HMENU) i, hInstance, NULL);

            SetScrollRange(hwndScroll[i], SB_CTL, 0, 255, FALSE);
            SetScrollPos(hwndScroll[i], SB_CTL, 0, FALSE);

            hwndLabel[i]    = CreateWindow(TEXT("static"), szColorLable[i],
                                           WS_CHILD | WS_VISIBLE | SS_CENTER,
                                           0, 0, 0, 0,
                                           hwnd, (HMENU) (i + 3),
                                           hInstance, NULL);

            hwndValue[i]    = CreateWindow(TEXT("static"), TEXT("0"),
                                           WS_CHILD | WS_VISIBLE | SS_CENTER,
                                           0, 0, 0, 0,
                                           hwnd, (HMENU) (i + 6),
                                           hInstance, NULL);

            OldScroll[i]    = (WNDPROC) SetWindowLongPtr(hwndScroll[i],
                                            GWLP_WNDPROC, (LONG)ScrollProc);

            hBrush[i]       = CreateSolidBrush(crPrim[i]);
        }

        hBrushStatic    = CreateSolidBrush(GetSysColor(COLOR_BTNHIGHLIGHT));
        cyChar          = HIWORD(GetDialogBaseUnits());
        return 0;

    case WM_SIZE:
        cxClient    = LOWORD(lParam);
        cyClient    = HIWORD(lParam);

        SetRect(&rcColor, cxClient / 2, 0, cxClient, cyClient);

        MoveWindow(hwndRect, 0, 0, cxClient / 2, cyClient, TRUE);

        for (i = 0; i < 3; i++) {
            MoveWindow(hwndScroll[i],
                       (2 * i + 1) * cxClient / 14, 2 * cyChar,
                       cxClient / 14, cyClient - 4 * cyChar, TRUE);

            MoveWindow(hwndLabel[i],
                       (4 * i + 1) * cxClient / 28, cyChar / 2,
                       cxClient / 7, cyChar, TRUE);

            MoveWindow(hwndValue[i],
                       (4 * i + 1) * cxClient / 28, cyClient - 3 * cyChar / 2,
                       cxClient / 7, cyChar, TRUE);
        }
        SetFocus(hwnd);
        return 0;
    case WM_SETFOCUS:
        SetFocus(hwndScroll[idFocus]);
        return 0;
    case WM_VSCROLL:
        i   = GetWindowLongPtr((HWND)lParam, GWLP_ID);

        switch (LOWORD(wParam)) {
        case SB_PAGEDOWN:
            color[i]    += 15;
        case SB_LINEDOWN:
            color[i]    = min(255, color[])
        case SB_PAGEUP:
        case SB_LINEUP:
        case SB_TOP:
        case SB_BOTTOM:
        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
        default:
        	break;
        }
        return 0;
    case WM_CTLCOLORSCROLLBAR:
        return 0;
    case WM_CTLCOLORSTATIC:
        return 0;
    case WM_SYSCOLORCHANGE:
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
    	break;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}