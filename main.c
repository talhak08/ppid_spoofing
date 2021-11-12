#include <windows.h>
#include <TlHelp32.h>

// wchat_t* is used for UTF-16 character space.
int getProcessId(HANDLE snapshot, wchar_t* processName) {
	PROCESSENTRY32 processEntry;
	// The size of the structure, in bytes.
	processEntry.dwSize = sizeof(PROCESSENTRY32);

	BOOL success = Process32First(snapshot, &processEntry);

	// Could not find a process.
	if (!success)
		return -2;

	while (success) {
		// wcscmp is used to compare UTF-16 strings.
		if (wcscmp(processEntry.szExeFile, processName) == 0)
			return processEntry.th32ProcessID;

		success = Process32Next(snapshot, &processEntry);
	}

	return -1;
}

int main(int argc, char** argv) {
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	HANDLE parrentProcessHandle = OpenProcess(MAXIMUM_ALLOWED, FALSE, getProcessId(snapshot, TEXT("explorer.exe")));
	CloseHandle(snapshot);

	STARTUPINFOEXA si;
	ZeroMemory(&si, sizeof(STARTUPINFOEXA));

	SIZE_T attrSize = 0;

	InitializeProcThreadAttributeList(NULL, 1, 0, &attrSize);
	si.lpAttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(GetProcessHeap(), 0, attrSize);
	InitializeProcThreadAttributeList(si.lpAttributeList, 1, 0, &attrSize);

	UpdateProcThreadAttribute(si.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_PARENT_PROCESS, &parrentProcessHandle, sizeof(HANDLE), NULL, NULL);
	si.StartupInfo.cb = sizeof(STARTUPINFOEXA);

	PROCESS_INFORMATION pi;
	CreateProcessA("C:\\Windows\\System32\\notepad.exe", NULL, NULL, NULL, FALSE, EXTENDED_STARTUPINFO_PRESENT, NULL, NULL, &si.StartupInfo, &pi);
	
	CloseHandle(parrentProcessHandle);
	return 0;
}