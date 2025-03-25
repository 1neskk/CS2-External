#pragma once

#include <Windows.h>
#include <TlHelp32.h>

namespace Memory
{
    DWORD GetProcessId(const WCHAR* processName);
}