#include <Windows.h>
#include <tchar.h>
#include <stdio.h>

int CDECL MessageBoxPrintf(TCHAR *szCaption, TCHAR *szFormat, ...)
{
    TCHAR szBuffer[1024];
    va_list pArgList;

    va_start(pArgList, szFormat);

    _vsntprintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR),
        szFormat, pArgList);

    va_end(pArgList);

    return MessageBox(0, szBuffer, szCaption, 0);
}

int WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd )
{
    int cxScreen, cyScreen;
    cxScreen = GetSystemMetrics(SM_CXSCREEN);
    cyScreen = GetSystemMetrics(SM_CYSCREEN);
    MessageBoxPrintf(TEXT("ScrnSize"),
        TEXT("The screen is %i pixels wide by %i pixels high."),
        cxScreen, cyScreen);

    BOOL fResult;
    int aMouseInfo[3];
    TCHAR szText[1024];
    fResult = GetSystemMetrics(SM_MOUSEPRESENT);

    if (fResult == 0) {
        _tcscpy(szText, TEXT("No mouse installed.\n"));
    } else {
        _tcscpy(szText, TEXT("Mouse installed.\n"));

        fResult = GetSystemMetrics(SM_SWAPBUTTON);

        if (fResult == 0) {
            _tcscat(szText, TEXT("Buttons not swapped.\n"));
        } else {
            _tcscat(szText, TEXT("Buttons swapped.\n"));
        }

        fResult = SystemParametersInfo(
            SPI_GETMOUSE,
            0,
            &aMouseInfo,
            0);

        if (fResult) {
            TCHAR szBuf[512];
            _stprintf(szBuf, TEXT("Speed: %d\nThreshold (x,y): %d, %d\n"), 
                aMouseInfo[2], aMouseInfo[0], aMouseInfo[1]);
            _tcscat(szText, szBuf);
        }
    }
    MessageBoxPrintf(TEXT("GetSystemMetrics Usage Example"), szText);
    return 0;
}