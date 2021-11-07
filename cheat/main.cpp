#include "memory.h"
#include <thread>

namespace offsets
{
	constexpr auto localPlayer = 0xDA544C;
	constexpr auto flags = 0x104;
	constexpr auto forceJump = 0x5269570;
	constexpr auto entityList = 0x4DBF75C;
	constexpr auto glowObjectManager = 0x5307C48;
	constexpr auto team = 0xF4;
	constexpr auto glowIndex = 0x10488;
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

		// bhop
		if (GetAsyncKeyState(VK_SPACE))
			(flags & (1 << 0)) ? mem.Write<std::uintptr_t>(client + offsets::forceJump, 6) : mem.Write<std::uintptr_t>(client + offsets::forceJump, 4);

		// glow
		const auto glowObjectManager = mem.Read<std::uintptr_t>(client + offsets::glowObjectManager);

		for (auto i = 0; i < 64; ++i)
		{
			const auto entity = mem.Read<std::uintptr_t>(client + offsets::entityList + i * 0x10);

			if (!entity)
				continue;

			// dont glow if they are on our team
			if (mem.Read<std::uintptr_t>(entity + offsets::team) == mem.Read<std::uintptr_t>(localPlayer + offsets::team))
				continue;

			const auto glowIndex = mem.Read<std::int32_t>(entity + offsets::glowIndex);

			mem.Write<float>(glowObjectManager + (glowIndex * 0x38) + 0x8, 1.f);
			mem.Write<float>(glowObjectManager + (glowIndex * 0x38) + 0xC, 0.f);
			mem.Write<float>(glowObjectManager + (glowIndex * 0x38) + 0x10, 0.f);
			mem.Write<float>(glowObjectManager + (glowIndex * 0x38) + 0x14, 1.f);

			mem.Write<bool>(glowObjectManager + (glowIndex * 0x38) + 0x28, true);
			mem.Write<bool>(glowObjectManager + (glowIndex * 0x38) + 0x29, true);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}
