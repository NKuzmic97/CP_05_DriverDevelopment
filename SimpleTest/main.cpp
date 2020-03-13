#include <Windows.h>
#include <stdio.h>
#include <tchar.h>

int _tmain(int argc, _TCHAR* argv[]) {
	HANDLE h = CreateFile(
		L"\\\\.\\mailslot\\MyMailBox",
		GENERIC_WRITE,
		FILE_SHARE_READ,
		nullptr,
		OPEN_EXISTING,
		0,
		nullptr);

	if (h == INVALID_HANDLE_VALUE) {
		printf("Error: %d\n", GetLastError());
		printf("Press any key to continue.. ");


		getchar();
		return 1;
	}

	WCHAR text[] = L"Test client says hi!";
	DWORD writtenBytes;

	WriteFile(h, text, sizeof(text), &writtenBytes, nullptr);
	CloseHandle(h);

	return 0;
}