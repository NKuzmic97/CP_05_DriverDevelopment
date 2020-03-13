#include <iostream>
#include <Windows.h>
#include <tchar.h>

void WINAPI MyServiceMain(DWORD count, LPSTR* args);

SERVICE_STATUS_HANDLE g_Handle = nullptr;
HANDLE g_hMailSlot = nullptr;

int _tmain(int argc, _TCHAR* argv[]) {
	SERVICE_TABLE_ENTRY table[] = {
		{(LPWSTR)L"MyService",(LPSERVICE_MAIN_FUNCTIONW)MyServiceMain},
		{nullptr,nullptr}
	};

	StartServiceCtrlDispatcher(table);
}

void SetServiceStatus(DWORD state) {
	static SERVICE_STATUS status;
	if (status.dwServiceType == 0) {
		status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	}
	status.dwCurrentState = state;
	SetServiceStatus(g_Handle, &status);
}

void WINAPI MyHandler(DWORD code) {
	switch (code) {
	case SERVICE_CONTROL_STOP:
		CloseHandle(g_hMailSlot);
		g_hMailSlot = nullptr;
		SetServiceStatus(SERVICE_STOPPED);
		break;
	
	default:
		break;
	}
}

void WINAPI MyServiceMain(DWORD count, LPSTR* args) {
	g_Handle = RegisterServiceCtrlHandler(L"MyService", MyHandler);
	SetServiceStatus(SERVICE_START_PENDING);

	SECURITY_DESCRIPTOR sd;
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, TRUE, nullptr, FALSE);
	SECURITY_ATTRIBUTES sa = { sizeof(sa) };
	sa.lpSecurityDescriptor = &sd;
	g_hMailSlot = CreateMailslot(L"\\\\.\\mailslot\MyMailBox", 1024, 100, &sa);
	SetServiceStatus(SERVICE_RUNNING);

	while (g_hMailSlot) {
		DWORD count, size, read;
		WCHAR buffer[512];

		if (!GetMailslotInfo(g_hMailSlot, nullptr, &size, &count, nullptr)) {
			break;
		}

		while (count > 0) {
			if (ReadFile(g_hMailSlot, buffer, size, &read, nullptr)) {
				OutputDebugString(buffer);
				GetMailslotInfo(g_hMailSlot, nullptr, &size, &count, nullptr);
			}
			else
				break;
		}
		Sleep(200);
	}

}
