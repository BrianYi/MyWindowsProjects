#include <windows.h>

#define ID_SMALLER      1
#define ID_LARGER       2

#define BTN_WIDTH        (8 * cxChar)
#define BTN_HEIGHT       (4 * cyChar)

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR szCmdLine, int iCmdShow)
{
    static TCHAR szAppName[] = TEXT ("OwnDraw") ;
    MSG          msg ;
    HWND         hwnd ;
    WNDCLASS     wndclass ;

    wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
    wndclass.lpfnWndProc   = WndProc ;
    wndclass.cbClsExtra    = 0 ;
    wndclass.cbWndExtra    = 0 ;
    wndclass.hInstance     = hInstance ;
    wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
    wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
    wndclass.lpszMenuName  = szAppName ;
    wndclass.lpszClassName = szAppName ;

    if (!RegisterClass (&wndclass))
    {
        MessageBox (NULL, TEXT ("This program requires Windows NT!"),
            szAppName, MB_ICONERROR) ;
        return 0 ;
    }

    hwnd = CreateWindow (szAppName, TEXT ("Owner-Draw Button Demo"),
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
	static HWND      hwndSmaller, hwndLarger ;
	static int       cxClient, cyClient, cxChar, cyChar ;
	int              cx, cy ;
	LPDRAWITEMSTRUCT pdis ;
	RECT             rc ;

    switch (message) {
    case WM_CREATE:
        cxChar  = LOWORD(GetDialogBaseUnits());
        cyChar  = HIWORD(GetDialogBaseUnits());

        hwndSmaller = CreateWindow(TEXT("BUTTON"), NULL, WS_VISIBLE | WS_CHILD |BS_OWNERDRAW,
                            0, 0, BTN_WIDTH, BTN_HEIGHT,
                            hwnd, (HMENU)ID_SMALLER, (LPCREATESTRUCT(lParam))->hInstance, NULL);

        hwndLarger  = CreateWindow(TEXT("BUTTON"), NULL, WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                            0, 0, BTN_WIDTH, BTN_HEIGHT,
                            hwnd, (HMENU)ID_LARGER, (LPCREATESTRUCT(lParam))->hInstance, NULL);
        return 0;
    case WM_SIZE:
        cxClient    = LOWORD(lParam);
        cyClient    = HIWORD(lParam);
        MoveWindow(hwndSmaller, cxClient / 2 - 3 * BTN_WIDTH / 2, cyClient / 2 - BTN_HEIGHT / 2, BTN_WIDTH, BTN_HEIGHT, TRUE);
        MoveWindow(hwndLarger,  cxClient / 2 + 1 * BTN_WIDTH / 2, cyClient / 2 - BTN_HEIGHT / 2, BTN_WIDTH, BTN_HEIGHT, TRUE);
        return 0;
    case WM_COMMAND:
        GetWindowRect(hwnd, &rc);
        if (wParam == ID_SMALLER) {
            rc.left     += cxClient / 20;
            rc.right    -= cxClient / 20;
            rc.top      += cyClient / 20;
            rc.bottom   -= cyClient / 20;
        } else if (wParam == ID_LARGER) {
            rc.left     -= cxClient / 20;
            rc.right    += cxClient / 20;
            rc.top      -= cyClient / 20;
            rc.bottom   += cyClient / 20;
        }
        MoveWindow(hwnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
        return 0;
    case WM_DRAWITEM:
        pdis    = (LPDRAWITEMSTRUCT)lParam;
        cx  = pdis->rcItem.right    - pdis->rcItem.left;
        cy  = pdis->rcItem.bottom   - pdis->rcItem.top;

        rc.left     = cx / 4;
        rc.right    = cx * 3 / 4;
        rc.top      = cy / 4;
        rc.bottom   = cy * 3 / 4;

        if (pdis->CtlID == ID_SMALLER) {
            FillRect(pdis->hDC, &pdis->rcItem, (HBRUSH)GetStockObject(WHITE_BRUSH));
            FillRect(pdis->hDC, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
        } else if (pdis->CtlID == ID_LARGER) {
            FillRect(pdis->hDC, &pdis->rcItem, (HBRUSH)GetStockObject(BLACK_BRUSH));
            FillRect(pdis->hDC, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
        }
        
        if (pdis->itemState & ODS_SELECTED) {
            InvertRect(pdis->hDC, &pdis->rcItem);
        }

        if (pdis->itemState & ODS_FOCUS) {
            rc.left     = pdis->rcItem.right / 16;
            rc.right    = pdis->rcItem.right - rc.left;
            rc.top      = pdis->rcItem.bottom / 16;
            rc.bottom   = pdis->rcItem.bottom - rc.top;
            DrawFocusRect(pdis->hDC, &rc);
        }
        FrameRect(pdis->hDC, &pdis->rcItem, (HBRUSH)GetStockObject(BLACK_BRUSH));

        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
    	break;
    }
	return DefWindowProc(hwnd, message, wParam, lParam);
}