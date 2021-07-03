#include "memory.h"
#include <thread>

namespace offsets
{
	constexpr auto localPlayer = 0xD892CC;
	constexpr auto flags = 0x104;
	constexpr auto forceJump = 0x524BF4C;
}

int main()
{
	auto mem = Memory("csgo.exe");

	std::cout << "Process id: " << mem.GetProcessId() << std::endl;

	const auto client = mem.GetModuleAddress("client.dll");
	std::cout << "client.dll -> " << "0x" << std::hex << client << std::dec << std::endl;

	while (true)
	{
		const auto localPlayer = mem.Read<uintptr_t>(client + offsets::localPlayer);

		if (localPlayer)
		{
			const auto onGround = mem.Read<bool>(localPlayer + offsets::flags);

			if (GetAsyncKeyState(VK_SPACE) && onGround & (1 << 0))
				mem.Write<BYTE>(client + offsets::forceJump, 6);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}
