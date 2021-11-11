#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>

#include <cstdint>
#include <string_view>

class Memory
{
private:
	std::uintptr_t processId = 0;
	void* processHandle = nullptr;

public:
	// Constructor that finds the process id
	// and opens a handle
	Memory(const std::string_view processName)
	{
		::PROCESSENTRY32 entry = { };
		entry.dwSize = sizeof(::PROCESSENTRY32);

		const auto snapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		while (::Process32Next(snapShot, &entry))
		{
			if (!processName.compare(entry.szExeFile))
			{
				processId = entry.th32ProcessID;
				processHandle = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
				break;
			}
		}

		if (snapShot) ::CloseHandle(snapShot);
	}

	// Destructor that frees the opened handle
	~Memory() { if (processHandle) ::CloseHandle(processHandle); }

	// Returns the base address of a module by name
	const std::uintptr_t GetModuleAddress(const std::string_view moduleName)
	{
		::MODULEENTRY32 entry = { };
		entry.dwSize = sizeof(::MODULEENTRY32);

		const auto snapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);

		std::uintptr_t result = 0;

		while (::Module32Next(snapShot, &entry))
		{
			if (!moduleName.compare(entry.szModule))
			{
				result = reinterpret_cast<std::uintptr_t>(entry.modBaseAddr);
				break;
			}
		}

		if (snapShot) ::CloseHandle(snapShot);

		return result;
	}

	// Read process memory
	template <typename T>
	constexpr T Read(std::uintptr_t address) const noexcept
	{
		T value{ };
		::ReadProcessMemory(processHandle, reinterpret_cast<LPCVOID>(address), &value, sizeof(T), NULL);
		return value;
	}

	// Write process memory
	template <typename T>
	constexpr const bool Write(std::uintptr_t address, T value) const noexcept
	{
		return ::WriteProcessMemory(processHandle, reinterpret_cast<LPVOID>(address), &value, sizeof(T), NULL);
	}
};
