#pragma once
#include <Windows.h>
#include <iostream>

class Memory
{
private:
	std::uintptr_t id = 0;
	HANDLE process = nullptr;

public:
	Memory(const char* processName);
	~Memory();

	constexpr std::uintptr_t ProcessId() { return this->id; }

	std::uintptr_t GetModuleAddress(const char* moduleName);

	template <typename T>
	constexpr T Read(std::uintptr_t address) const noexcept
	{
		T value{ };
		ReadProcessMemory(this->process, reinterpret_cast<LPCVOID>(address), &value, sizeof(T), NULL);
		return value;
	}

	template <typename T>
	constexpr const bool Write(std::uintptr_t address, T value) const noexcept
	{
		return WriteProcessMemory(this->process, reinterpret_cast<LPVOID>(address), &value, sizeof(T), NULL);
	}
};
