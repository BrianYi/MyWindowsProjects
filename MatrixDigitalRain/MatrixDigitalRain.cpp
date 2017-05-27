#include <windows.h>
#include <list>
using std::list;
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
    SetTimer(hwnd, 1, 10, NULL);
    srand(GetCurrentTime());
	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}
	return msg.wParam ;
}

struct MatrixData
{
    MatrixData()
    {
        cClientPos  = 0;
        cLen        = 0;
        cPos        = 0;
        szBuffer    = NULL;
    }
    ~MatrixData()
    {
        free(szBuffer);
    }
    unsigned int cClientPos;
    unsigned int cLen;
    unsigned int cPos;
    TCHAR   *szBuffer;
};

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	PAINTSTRUCT	ps;
	static int	cxClient, cyClient, cxChar, cyChar, cxCaps;
	static TCHAR szMatrix[] = TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
    static int  cMatrixSize = lstrlen(szMatrix);
	static HFONT   hFont;
    int         cClientPos, cLen, i, j, r, g, b;
    static int  cMinLength = 5, cMaxLength = 50;
    static list<MatrixData*>   listMatrixInfo;
    bool        isRepick    = false;
	switch (message) 
	{
    case WM_TIMER:
        for (i = 1; i <= 15;) {
            if (listMatrixInfo.size() >= (cxClient / cxChar))
                break;
            cClientPos = rand() % (cxClient / cxChar);
            isRepick = false;
//             for (auto it = listMatrixInfo.begin(); it != listMatrixInfo.end(); it++) 
//             {
//                 if ((*it)->cClientPos == cClientPos)
//                 {
//                     isRepick = true;
//                     break;
//                 }
//             }
//             if (isRepick) 
//             {
//                 continue;
//             }
            cLen = cMinLength + rand() % (cMaxLength - cMinLength + 1);    // 15~30
            MatrixData *matrix  = new MatrixData;
            matrix->cPos        = 0;
            matrix->cClientPos  = cClientPos;
            matrix->cLen        = cLen;
            matrix->szBuffer    = new TCHAR[cLen];
            for (j = 0; j < cLen; j++)
                matrix->szBuffer[j] = szMatrix[rand() % cMatrixSize];
            listMatrixInfo.push_back(matrix);
            i++;
        }
        ScrollWindow(hwnd, 0, cyChar, NULL, NULL);
        return 0;

	case WM_CREATE:
		hFont = CreateFont(20, 10, -900, 0, FW_MEDIUM, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, CLEARTYPE_QUALITY, FF_DONTCARE, NULL);
        cxChar = 20, cyChar = 10;
//         cxClient    = GetSystemMetrics(SM_CXMAXIMIZED);
//         cyClient    = GetSystemMetrics(SM_CYMAXIMIZED);
		return 0;

	case WM_SIZE:
		cxClient	= LOWORD(lParam);
		cyClient	= HIWORD(lParam);
		return 0;

	case WM_PAINT:
		hdc	= BeginPaint(hwnd, &ps);
		SelectObject(hdc, hFont);
 		SetBkMode(hdc, TRANSPARENT);
        for (auto it = listMatrixInfo.begin(); it != listMatrixInfo.end(); ) 
        {
            MatrixData *matrix = *it;
            // pos 0~14 => (255,255,255)~(0,255,0)
            if (matrix->cPos < 1) 
            {
                r   = (matrix->cLen - matrix->cPos) * 255.0 / matrix->cLen;
                g   = 255;
                b   = r;
                
            }
            else 
            {
                r   = 0;
                g   = (matrix->cLen - matrix->cPos) * 255.0 / (matrix->cLen);
                b   = 0;
            }
            SetTextColor(hdc, RGB(r, g, b));
            TextOut(hdc, matrix->cClientPos * cxChar, 0, &matrix->szBuffer[matrix->cPos], 1);
            // pos 15~29 => (0,255,0)~(0,0,0)
            // del matrix
            matrix->cPos++;
            if (matrix->cPos == matrix->cLen) 
             {
                 free(*it);
                 it = listMatrixInfo.erase(it);
            }
             else 
            	it++;
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