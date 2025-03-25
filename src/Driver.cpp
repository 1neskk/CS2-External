#include "Driver.h"

uintptr_t Driver::GetModuleBaseAddress(DWORD ProcessId, const char* ModuleName)
{
	Request request = {};
	request.processId = ProcessId;
	request.requestBase = TRUE;
	request.read = FALSE;
	request.write = FALSE;
	request.moduleName = ModuleName;

	CallHook(&request);

	const uintptr_t baseAddress = request.baseAddress;
	return baseAddress;
}

bool Driver::Write(DWORD ProcessId, uintptr_t Address, uintptr_t SourceAddress, size_t Size)
{
	Request request = {};
	request.target = Address;
	request.processId = ProcessId;
	request.write = true;
	request.read = false;
	request.requestBase = false;
	request.buffer = reinterpret_cast<PVOID>(SourceAddress);
	request.size = Size;

	CallHook(&request);

	return true;
}
