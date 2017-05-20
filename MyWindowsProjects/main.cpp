#include <Windows.h>

int WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd )
{
    MessageBox (NULL, TEXT("Hello, Windows 2007!"), TEXT("HelloMsg"), 0);
    return 0;
}