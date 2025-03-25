#pragma once
#include "Memory.h"
#include <cstdint>
#include <iostream>

namespace Driver
{
	struct Request
	{
		DWORD processId;
		uintptr_t baseAddress;
		const char* moduleName;

		PVOID buffer;
		uintptr_t target;
		uintptr_t size;

		BOOLEAN read, write;
		BOOLEAN requestBase;

		PVOID result;
	};

	template <typename ... Arg>
	uint64_t CallHook(const Arg ... args)
	{
		LoadLibraryA("user32.dll");

		const HMODULE hModule = LoadLibraryA("win32u.dll");
		if (!hModule)
		{
			std::cerr << "[-] Failed to load win32u.dll, error: " << GetLastError() << "\n";
			return 0;
		}

		void* hook = GetProcAddress(hModule, "NtOpenCompositionSurfaceSectionInfo");
		if (!hook)
		{
			std::cerr << "[-] Failed to get address of NtOpenCompositionSurfaceSectionInfo, error: " << GetLastError() << "\n";
			FreeLibrary(hModule);
			return 0;
		}

		auto function = static_cast<uint64_t(__stdcall*)(Arg...)>(hook);
		const uint64_t result = function(args...);

		return result;
	}

	uintptr_t GetModuleBaseAddress(DWORD ProcessId, const char* ModuleName);
	bool Write(DWORD ProcessId, uintptr_t Address, uintptr_t SourceAddress, size_t Size);

	template <typename T >
	T Read(const DWORD ProcessId, const uintptr_t Address)
	{
		T response{};
		Request request = {};
		request.processId = ProcessId;
		request.size = sizeof(T);
		request.target = Address;
		request.read = true;
		request.write = false;
		request.requestBase = false;
		request.result = &response;

		CallHook(&request);

		return response;
	}

	template <typename T>
	bool Write(const DWORD ProcessId, const uintptr_t Address, const T& Value)
	{
		return Write(ProcessId, Address, reinterpret_cast<uintptr_t>(&Value), sizeof(T));
	}
}
