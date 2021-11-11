#include "memory.h"
#include <TlHelp32.h>

Memory::Memory(const char* processName)
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	while (Process32Next(snapShot, &entry))
	{
		if (!strcmp(processName, entry.szExeFile))
		{
			this->id = entry.th32ProcessID;
			this->process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->id);
			break;
		}
	}

	if (snapShot)
		CloseHandle(snapShot);
}

Memory::~Memory()
{
	if (this->process)
		CloseHandle(this->process);
}

std::uintptr_t Memory::GetModuleAddress(const char* moduleName)
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
