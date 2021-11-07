#include "memory.h"
#include <thread>

namespace offsets
{
	constexpr auto localPlayer = 0xDA544C;
	constexpr auto flags = 0x104;
	constexpr auto forceJump = 0x5269570;
}

int main()
{
	auto mem = Memory("csgo.exe");

	std::cout << "Process id: " << mem.GetProcessId() << std::endl;

	const auto client = mem.GetModuleAddress("client.dll");
	std::cout << "client.dll -> " << "0x" << std::hex << client << std::dec << std::endl;

	while (true)
	{
		const auto localPlayer = mem.Read<std::uintptr_t>(client + offsets::localPlayer);
		const auto flags = mem.Read<std::uintptr_t>(localPlayer + offsets::flags);

		if (GetAsyncKeyState(VK_SPACE))
			(flags & (1 << 0)) ? mem.Write<std::uintptr_t>(client + offsets::forceJump, 6) : mem.Write<std::uintptr_t>(client + offsets::forceJump, 4);

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}
