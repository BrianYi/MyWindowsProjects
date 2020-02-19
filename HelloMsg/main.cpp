#include <Windows.h>
#include <tchar.h>
int CALLBACK WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd )
{
	static wchar_t a[] = L"Hello!";
	wchar_t c = L'A';

    MessageBox (NULL, TEXT("Hello, Windows 2007!"), TEXT("HelloMsg"),MB_HELP | MB_ICONASTERISK );
    return 0;
}