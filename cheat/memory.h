#pragma once
#include <Windows.h>
#include <iostream>

class Memory
{
private:
	DWORD id = 0;
	HANDLE process = nullptr;

public:
	Memory(const char* processName);
	~Memory();

	constexpr DWORD GetProcessId() { return id; }
	constexpr HANDLE GetProcessHandle() { return process; }

	uintptr_t GetModuleAddress(const char* moduleName);

	template <typename T>
	T Read(uintptr_t address)
	{
		T value;
		ReadProcessMemory(this->process, (LPCVOID)address, &value, sizeof(T), NULL);
		return value;
	}

	template <typename T>
	bool Write(uintptr_t address, T value)
	{
		return WriteProcessMemory(this->process, (LPVOID)address, &value, sizeof(T), NULL);
	}
};
