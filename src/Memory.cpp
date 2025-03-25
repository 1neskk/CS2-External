#include "Memory.h"

DWORD Memory::GetProcessId(const WCHAR* processName)
{
	PROCESSENTRY32W entry;
	entry.dwSize = sizeof(PROCESSENTRY32W);

	const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (snapshot == INVALID_HANDLE_VALUE)
		return 0;

	DWORD pid = 0;

	if (Process32FirstW(snapshot, &entry))
	{
		do
		{
			if (!wcscmp(entry.szExeFile, processName))
			{
				pid = entry.th32ProcessID;
				break;
			}
		} while (Process32NextW(snapshot, &entry));
	}

	CloseHandle(snapshot);
	return pid;
}
