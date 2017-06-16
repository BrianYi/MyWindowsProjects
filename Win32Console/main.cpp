//下面的代码没有考虑源数据的长度问题，需要注意。
// INCLUDES
#include "stdio.h"
#include "conio.h"
#include "windows.h"
#include "wincrypt.h"
#include "tchar.h"

// FUNCTIONS
int Keys(_TCHAR* strPublicKeyFile, _TCHAR* strPrivateKeyFile);
int Encrypt(_TCHAR* strPublicKeyFile, _TCHAR* strPlainFile, _TCHAR* strEncryptedFile);
int Decrypt(_TCHAR* strPrivateKeyFile, _TCHAR* strEncryptedFile, _TCHAR* strPlainFile);

// Main
int _tmain(int argc, _TCHAR* argv[])
{
	int iResult = 0;

	if ((argc == 4) && (_tcscmp(argv[1], _T("k")) == 0))
	{
		// Generate a new key pair
		iResult = Keys(argv[2], argv[3]);
	}
	else if ((argc == 5) && (_tcscmp(argv[1], _T("e")) == 0))
	{
		// Encrypt
		iResult = Encrypt(argv[2], argv[3], argv[4]);
	}
	else if ((argc == 5) && (_tcscmp(argv[1], _T("d"))== 0))
	{
		// Decrypt
		iResult = Decrypt(argv[2], argv[3], argv[4]);
	}
	else 
	{
		// Show usage
		_tprintf(_T("Usage:\n"));
		_tprintf(_T("   - New key pair: EncryptDecrypt k public_key_file private_key_file\n"));
		_tprintf(_T("   - Encrypt:      EncryptDecrypt e public_key_file plain_file encrypted_file\n"));
		_tprintf(_T("   - Decrypt:      EncryptDecrypt d private_key_file encrypted_file plain_file\n"));
		iResult = 1;
	}

// 	_tprintf(_T("\n<< Press any key to continue >>\n"));
// 	_getch();
	return iResult;
}
// End of Main

// Keys
int Keys(_TCHAR* strPublicKeyFile, _TCHAR* strPrivateKeyFile)
{
	// Variables
	HCRYPTPROV hCryptProv = NULL;
	HCRYPTKEY hKey = NULL;
	DWORD dwPublicKeyLen = 0;
	DWORD dwPrivateKeyLen = 0;
	BYTE* pbPublicKey = NULL;
	BYTE* pbPrivateKey = NULL;
	HANDLE hPublicKeyFile = NULL;
	HANDLE hPrivateKeyFile = NULL;
	DWORD lpNumberOfBytesWritten = 0;

	__try 
	{
		// Acquire access to key container
		_tprintf(_T("CryptAcquireContext...\n"));
		if (!CryptAcquireContext(&hCryptProv, _T("AlejaCMa.EncryptDecrypt"), NULL, PROV_RSA_FULL, 0)) 
		{
			// Error
			_tprintf(_T("CryptAcquireContext error 0x%x\n"), GetLastError());

			// Try to create a new key container
			if (!CryptAcquireContext(&hCryptProv, _T("AlejaCMa.EncryptDecrypt"), NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET))
			{
				// Error
				_tprintf(_T("CryptAcquireContext error 0x%x\n"), GetLastError());
				return 1;
			}
		}

		// Generate new key pair
		_tprintf(_T("CryptGenKey...\n"));
		if (!CryptGenKey(hCryptProv, AT_KEYEXCHANGE,  CRYPT_ARCHIVABLE, &hKey))
		{
			// Error
			_tprintf(_T("CryptGenKey error 0x%x\n"), GetLastError());
			return 1;
		}

		// Get public key size
		_tprintf(_T("CryptExportKey...\n"));
		if (!CryptExportKey(hKey, NULL, PUBLICKEYBLOB, 0, NULL, &dwPublicKeyLen))
		{
			// Error
			_tprintf(_T("CryptExportKey error 0x%x\n"), GetLastError());
			return 1;
		}

		// Create a buffer for the public key
		_tprintf(_T("malloc...\n"));
		if (!(pbPublicKey = (BYTE *)malloc(dwPublicKeyLen)))
		{
			// Error
			_tprintf(_T("malloc error 0x%x\n"), GetLastError());
			return 1;
		}

		// Get public key
		_tprintf(_T("CryptExportKey...\n"));
		if (!CryptExportKey(hKey, NULL, PUBLICKEYBLOB, 0, pbPublicKey, &dwPublicKeyLen))
		{
			// Error
			_tprintf(_T("CryptExportKey error 0x%x\n"), GetLastError());
			return 1;
		}

		// Get private key size
		_tprintf(_T("CryptExportKey...\n"));
		if (!CryptExportKey(hKey, NULL, PRIVATEKEYBLOB, 0, NULL, &dwPrivateKeyLen))
		{
			// Error
			_tprintf(_T("CryptExportKey error 0x%x\n"), GetLastError());
			return 1;
		}

		// Create a buffer for the private key
		_tprintf(_T("malloc...\n"));
		if (!(pbPrivateKey = (BYTE *)malloc(dwPrivateKeyLen)))
		{
			// Error
			_tprintf(_T("malloc error 0x%x\n"), GetLastError());
			return 1;
		}

		// Get private key
		_tprintf(_T("CryptExportKey...\n"));
		if (!CryptExportKey(hKey, NULL, PRIVATEKEYBLOB, 0, pbPrivateKey, &dwPrivateKeyLen))
		{
			// Error
			_tprintf(_T("CryptExportKey error 0x%x\n"), GetLastError());
			return 1;
		}

		// Create a file to save the public key
		_tprintf(_T("CreateFile...\n"));
		if ((hPublicKeyFile = CreateFile(
			strPublicKeyFile,
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			)) == INVALID_HANDLE_VALUE)
		{
			// Error
			_tprintf(_T("CreateFile error 0x%x\n"), GetLastError());
			return 1;
		}

		// Write the public key to the file
		_tprintf(_T("WriteFile...\n"));
		if (!WriteFile(
			hPublicKeyFile, 
			(LPCVOID)pbPublicKey, 
			dwPublicKeyLen, 
			&lpNumberOfBytesWritten, 
			NULL
			)) 
		{
			// Error
			_tprintf(_T("WriteFile error 0x%x\n"), GetLastError());
			return 1;
		}

		// Create a file to save the private key
		_tprintf(_T("CreateFile...\n"));
		if ((hPrivateKeyFile = CreateFile(
			strPrivateKeyFile,
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			)) == INVALID_HANDLE_VALUE)
		{
			// Error
			_tprintf(_T("CreateFile error 0x%x\n"), GetLastError());
			return 1;
		}

		// Write the private key to the file
		_tprintf(_T("WriteFile...\n"));
		if (!WriteFile(
			hPrivateKeyFile, 
			(LPCVOID)pbPrivateKey, 
			dwPrivateKeyLen, 
			&lpNumberOfBytesWritten, 
			NULL
			)) 
		{
			// Error
			_tprintf(_T("WriteFile error 0x%x\n"), GetLastError());
			return 1;
		}

		return 0;
	}
	__finally
	{
		// Clean up       
		if (!pbPublicKey) {
			_tprintf(_T("free...\n"));
			free(pbPublicKey);
		}           
		if (!pbPrivateKey) {
			_tprintf(_T("free...\n"));
			free(pbPrivateKey);
		}           
		if (hPublicKeyFile) {
			_tprintf(_T("CloseHandle...\n"));
			CloseHandle(hPublicKeyFile);
		}           
		if (hPrivateKeyFile) {
			_tprintf(_T("CloseHandle...\n"));
			CloseHandle(hPrivateKeyFile);
		}
		if (hKey) {
			_tprintf(_T("CryptDestroyKey...\n"));
			CryptDestroyKey(hKey);
		}           
		if (hCryptProv) {
			_tprintf(_T("CryptReleaseContext...\n"));
			CryptReleaseContext(hCryptProv, 0);
		}
	}
}
// End of Keys

// Encrypt
int Encrypt(_TCHAR* strPublicKeyFile, _TCHAR* strPlainFile, _TCHAR* strEncryptedFile)
{
	// Variables
	HCRYPTPROV hCryptProv = NULL;
	HCRYPTKEY hKey = NULL;
	DWORD dwPublicKeyLen = 0;
	DWORD dwDataLen = 0;
	DWORD dwEncryptedLen = 0;
	BYTE* pbPublicKey = NULL;
	BYTE* pbData = NULL;
	HANDLE hPublicKeyFile = NULL;
	HANDLE hEncryptedFile = NULL;
	HANDLE hPlainFile = NULL;
	DWORD lpNumberOfBytesWritten = 0;

	__try
	{
		// Acquire access to key container
		_tprintf(_T("CryptAcquireContext...\n"));
		if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) 
		{
			// Error
			_tprintf(_T("CryptAcquireContext error 0x%x\n"), GetLastError());
			return 1;
		}

		// Open public key file
		_tprintf(_T("CreateFile...\n"));
		if ((hPublicKeyFile = CreateFile(
			strPublicKeyFile,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_SEQUENTIAL_SCAN,
			NULL
			)) == INVALID_HANDLE_VALUE)
		{
			// Error
			_tprintf(_T("CreateFile error 0x%x\n"), GetLastError());
			return 1;
		}

		// Get file size
		_tprintf(_T("GetFileSize...\n"));
		if ((dwPublicKeyLen = GetFileSize(hPublicKeyFile, NULL)) == INVALID_FILE_SIZE)
		{
			// Error
			_tprintf(_T("GetFileSize error 0x%x\n"), GetLastError());
			return 1;
		}

		// Create a buffer for the public key
		_tprintf(_T("malloc...\n"));
		if (!(pbPublicKey = (BYTE *)malloc(dwPublicKeyLen)))
		{
			// Error
			_tprintf(_T("malloc error 0x%x\n"), GetLastError());
			return 1;
		}

		// Read public key
		_tprintf(_T("ReadFile...\n"));
		if (!ReadFile(hPublicKeyFile, pbPublicKey, dwPublicKeyLen, &dwPublicKeyLen, NULL))
		{
			// Error
			_tprintf(_T("ReadFile error 0x%x\n"), GetLastError());
			return 1;
		}

		// Import public key
		_tprintf(_T("CryptImportKey...\n"));
		if (!CryptImportKey(hCryptProv, pbPublicKey, dwPublicKeyLen, 0, 0, &hKey))
		{
			// Error
			_tprintf(_T("CryptImportKey error 0x%x\n"), GetLastError());
			return 1;
		}

		// Open plain text file
		_tprintf(_T("CreateFile...\n"));
		if ((hPlainFile = CreateFile(
			strPlainFile,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_SEQUENTIAL_SCAN,
			NULL
			)) == INVALID_HANDLE_VALUE)
		{
			// Error
			_tprintf(_T("CreateFile error 0x%x\n"), GetLastError());
			return 1;
		}

		// Get file size
		_tprintf(_T("GetFileSize...\n"));
		if ((dwDataLen = GetFileSize(hPlainFile, NULL)) == INVALID_FILE_SIZE)
		{
			// Error
			_tprintf(_T("GetFileSize error 0x%x\n"), GetLastError());
			return 1;
		}

		// Create a buffer for the plain text
		_tprintf(_T("malloc...\n"));
		if (!(pbData = (BYTE *)malloc(dwDataLen)))
		{
			// Error
			_tprintf(_T("malloc error 0x%x\n"), GetLastError());
			return 1;
		}

		// Read plain text
		_tprintf(_T("ReadFile...\n"));
		if (!ReadFile(hPlainFile, pbData, dwDataLen, &dwDataLen, NULL))
		{
			// Error
			_tprintf(_T("ReadFile error 0x%x\n"), GetLastError());
			return 1;
		}

		// Get lenght for encrypted data
		if (!CryptEncrypt(hKey, NULL, TRUE, 0, NULL, &dwEncryptedLen, 0))
		{
			// Error
			_tprintf(_T("CryptEncrypt error 0x%x\n"), GetLastError());
			return 1;
		}

		// Create a buffer for encrypted data
		_tprintf(_T("malloc...\n"));

		if (!(pbData = (BYTE *)realloc(pbData, dwEncryptedLen)))
		{
			// Error
			_tprintf(_T("malloc error 0x%x\n"), GetLastError());
			return 1;
		}

		// Encrypt data
		if (!CryptEncrypt(hKey, NULL, TRUE, 0, pbData, &dwDataLen, dwEncryptedLen))
		{
			DWORD dwErr = GetLastError();
			if (dwErr == ERROR_INVALID_HANDLE) 
			{
			}
			else if (dwErr == ERROR_INVALID_PARAMETER) 
			{
			}
			else if (dwErr == NTE_BAD_ALGID) 
			{
			}
			else if (dwErr == NTE_BAD_DATA) 
			{
			}
			else if (dwErr == NTE_BAD_FLAGS) 
			{
			}

			// Error
			_tprintf(_T("CryptEncrypt error 0x%x\n"), GetLastError());
			return 1;
		}

		// Create a file to save the encrypted data
		_tprintf(_T("CreateFile...\n"));
		if ((hEncryptedFile = CreateFile(
			strEncryptedFile,
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			)) == INVALID_HANDLE_VALUE)
		{
			// Error
			_tprintf(_T("CreateFile error 0x%x\n"), GetLastError());
			return 1;
		}

		// Write the public key to the file
		_tprintf(_T("WriteFile...\n"));
		if (!WriteFile(
			hEncryptedFile, 
			(LPCVOID)pbData, 
			dwDataLen, 
			&lpNumberOfBytesWritten, 
			NULL
			)) 
		{
			// Error
			_tprintf(_T("WriteFile error 0x%x\n"), GetLastError());
			return 1;
		}

		return 0;
	}
	__finally
	{
		// Clean up
		if (!pbPublicKey) {
			_tprintf(_T("free...\n"));
			free(pbPublicKey);
		}
		if (!pbData) {
			_tprintf(_T("free...\n"));
			free(pbData);
		}
		if (hPublicKeyFile) {
			_tprintf(_T("CloseHandle...\n"));
			CloseHandle(hPublicKeyFile);
		}
		if (hPlainFile) {
			_tprintf(_T("CloseHandle...\n"));
			CloseHandle(hPlainFile);
		}
		if (hEncryptedFile) {
			_tprintf(_T("CloseHandle...\n"));
			CloseHandle(hEncryptedFile);
		}
		if (hKey) {
			_tprintf(_T("CryptDestroyKey...\n"));
			CryptDestroyKey(hKey);
		}           
		if (hCryptProv) {
			_tprintf(_T("CryptReleaseContext...\n"));
			CryptReleaseContext(hCryptProv, 0);
		}
	}
}
// End of Encrypt

// Decrypt
int Decrypt(_TCHAR* strPrivateKeyFile, _TCHAR* strEncryptedFile, _TCHAR* strPlainFile)
{
	// Variables
	HCRYPTPROV hCryptProv = NULL;
	HCRYPTKEY hKey = NULL;
	DWORD dwPrivateKeyLen = 0;
	DWORD dwDataLen = 0;
	BYTE* pbPrivateKey = NULL;
	BYTE* pbData = NULL;
	HANDLE hPrivateKeyFile = NULL;
	HANDLE hEncryptedFile = NULL;
	HANDLE hPlainFile = NULL;
	DWORD lpNumberOfBytesWritten = 0;

	__try 
	{
		// Acquire access to key container
		_tprintf(_T("CryptAcquireContext...\n"));
		if (!CryptAcquireContext(&hCryptProv, _T("AlejaCMa.EncryptDecrypt"), NULL, PROV_RSA_FULL, 0)) 
		{
			// Error
			_tprintf(_T("CryptAcquireContext error 0x%x\n"), GetLastError());

			// Try to create a new key container
			if (!CryptAcquireContext(&hCryptProv, _T("AlejaCMa.EncryptDecrypt"), NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET))
			{
				// Error
				_tprintf(_T("CryptAcquireContext error 0x%x\n"), GetLastError());
				return 1;
			}
		}

		// Open private key file
		_tprintf(_T("CreateFile...\n"));
		if ((hPrivateKeyFile = CreateFile(
			strPrivateKeyFile,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_SEQUENTIAL_SCAN,
			NULL
			)) == INVALID_HANDLE_VALUE)
		{
			// Error
			_tprintf(_T("CreateFile error 0x%x\n"), GetLastError());
			return 1;
		}

		// Get file size
		_tprintf(_T("GetFileSize...\n"));
		if ((dwPrivateKeyLen = GetFileSize(hPrivateKeyFile, NULL)) == INVALID_FILE_SIZE)
		{
			// Error
			_tprintf(_T("GetFileSize error 0x%x\n"), GetLastError());
			return 1;
		}

		// Create a buffer for the private key
		_tprintf(_T("malloc...\n"));
		if (!(pbPrivateKey = (BYTE *)malloc(dwPrivateKeyLen)))
		{
			// Error
			_tprintf(_T("malloc error 0x%x\n"), GetLastError());
			return 1;
		}

		// Read private key
		_tprintf(_T("ReadFile...\n"));
		if (!ReadFile(hPrivateKeyFile, pbPrivateKey, dwPrivateKeyLen, &dwPrivateKeyLen, NULL))
		{
			// Error
			_tprintf(_T("ReadFile error 0x%x\n"), GetLastError());
			return 1;
		}

		// Import private key
		_tprintf(_T("CryptImportKey...\n"));
		if (!CryptImportKey(hCryptProv, pbPrivateKey, dwPrivateKeyLen, 0, 0, &hKey))
		{
			// Error
			_tprintf(_T("CryptImportKey error 0x%x\n"), GetLastError());
			return 1;
		}

		// Open encrypted file
		_tprintf(_T("CreateFile...\n"));
		if ((hEncryptedFile = CreateFile(
			strEncryptedFile,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_SEQUENTIAL_SCAN,
			NULL
			)) == INVALID_HANDLE_VALUE)
		{
			// Error
			_tprintf(_T("CreateFile error 0x%x\n"), GetLastError());
			return 1;
		}

		// Get file size
		_tprintf(_T("GetFileSize...\n"));
		if ((dwDataLen = GetFileSize(hEncryptedFile, NULL)) == INVALID_FILE_SIZE)
		{
			// Error
			_tprintf(_T("GetFileSize error 0x%x\n"), GetLastError());
			return 1;
		}

		// Create a buffer for the encrypted data
		_tprintf(_T("malloc...\n"));
		if (!(pbData = (BYTE *)malloc(dwDataLen)))
		{
			// Error
			_tprintf(_T("malloc error 0x%x\n"), GetLastError());
			return 1;
		}

		// Read encrypted data
		_tprintf(_T("ReadFile...\n"));
		if (!ReadFile(hEncryptedFile, pbData, dwDataLen, &dwDataLen, NULL))
		{
			// Error
			_tprintf(_T("ReadFile error 0x%x\n"), GetLastError());
			return 1;
		}

		// Get lenght for plain text
		if (!CryptDecrypt(hKey, NULL, TRUE, 0, pbData, &dwDataLen))
		{
			// Error
			_tprintf(_T("CryptDecrypt error 0x%x\n"), GetLastError());
			return 1;
		}

		// Create a file to save the plain text
		_tprintf(_T("CreateFile...\n"));
		if ((hPlainFile = CreateFile(
			strPlainFile,
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			)) == INVALID_HANDLE_VALUE)
		{
			// Error
			_tprintf(_T("CreateFile error 0x%x\n"), GetLastError());
			return 1;
		}

		// Write the plain text the file
		_tprintf(_T("WriteFile...\n"));
		if (!WriteFile(
			hPlainFile, 
			(LPCVOID)pbData, 
			dwDataLen, 
			&lpNumberOfBytesWritten, 
			NULL
			)) 
		{
			// Error
			_tprintf(_T("WriteFile error 0x%x\n"), GetLastError());
			return 1;
		}

		return 0;
	}
	__finally
	{
		// Clean up       
		if (!pbPrivateKey) {
			_tprintf(_T("free...\n"));
			free(pbPrivateKey);
		}                       
		if (!pbData) {
			_tprintf(_T("free...\n"));
			free(pbData);
		}           
		if (hPrivateKeyFile) {
			_tprintf(_T("CloseHandle...\n"));
			CloseHandle(hPrivateKeyFile);
		}
		if (hEncryptedFile) {
			_tprintf(_T("CloseHandle...\n"));
			CloseHandle(hEncryptedFile);
		}
		if (hPlainFile) {
			_tprintf(_T("CloseHandle...\n"));
			CloseHandle(hPlainFile);
		}
		if (hKey) {
			_tprintf(_T("CryptDestroyKey...\n"));
			CryptDestroyKey(hKey);
		}           
		if (hCryptProv) {
			_tprintf(_T("CryptReleaseContext...\n"));
			CryptReleaseContext(hCryptProv, 0);
		}
	}
}
// End of Decrypt