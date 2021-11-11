#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>

#include <cstdint>

class Memory
{
private:
	std::uintptr_t id = 0;
	HANDLE process = nullptr;

public:
	// Constructor that finds the process id
	// and opens a handle
	Memory(const char* processName)
	{
		PROCESSENTRY32 entry;
		entry.dwSize = sizeof(PROCESSENTRY32);

		const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		while (Process32Next(snapShot, &entry))
		{
			if (!strcmp(processName, entry.szExeFile))
			{
				id = entry.th32ProcessID;
				process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, id);
				break;
			}
		}

		if (snapShot)
			CloseHandle(snapShot);
	}

	// Destructor that frees the opened handle
	~Memory()
	{
		if (process)
			CloseHandle(process);
	}

	// Returns the base address of a module by name
	const std::uintptr_t GetModuleAddress(const char* moduleName)
	{
		MODULEENTRY32 entry;
		entry.dwSize = sizeof(MODULEENTRY32);

		const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, this->id);

		std::uintptr_t result = 0;

		while (Module32Next(snapShot, &entry))
		{
			if (!strcmp(moduleName, entry.szModule))
			{
				result = reinterpret_cast<std::uintptr_t>(entry.modBaseAddr);
				break;
			}
		}

		if (snapShot)
			CloseHandle(snapShot);

		return result;
	}

	// Read process memory
	template <typename T>
	constexpr T Read(std::uintptr_t address) const noexcept
	{
		T value{ };
		ReadProcessMemory(process, reinterpret_cast<LPCVOID>(address), &value, sizeof(T), NULL);
		return value;
	}

	// Write process memory
	template <typename T>
	constexpr const bool Write(std::uintptr_t address, T value) const noexcept
	{
		return WriteProcessMemory(process, reinterpret_cast<LPVOID>(address), &value, sizeof(T), NULL);
	}
};
