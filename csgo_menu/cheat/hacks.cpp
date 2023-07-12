#include "hacks.h"
#include "gui.h"
#include "offsets.h"

void hacks::VisualsThread(const Memory& memory) noexcept
{
	while (gui::isRunning)
	{
		const auto localPlayer = memory.Read<std::uintptr_t>(globals::clientAdress + hazedumper::signatures::dwLocalPlayer);
		if (!localPlayer) continue;
		const auto glowObjectManager = memory.Read<std::uintptr_t>(globals::clientAdress + hazedumper::signatures::dwGlowObjectManager);
		if (!glowObjectManager) continue;
		const auto localTeam = memory.Read<std::int32_t>(localPlayer + hazedumper::netvars::m_iTeamNum);

		for (auto i = 1; i <= 32; ++i)
		{
			const auto entity = memory.Read<std::uintptr_t>(globals::clientAdress + hazedumper::signatures::dwEntityList + i * 0x10);
			if (!entity) continue;
			if (entity == localPlayer) continue;

			const auto lifeState = memory.Read<std::int32_t>(entity + hazedumper::netvars::m_lifeState);
			if (lifeState != 0) continue;
			const auto entityTeam = memory.Read<std::int32_t>(entity + hazedumper::netvars::m_iTeamNum);

			if (globals::glowTeam && entityTeam == localTeam)
			{
				const auto& glowIndex = memory.Read<std::int32_t>(entity + hazedumper::netvars::m_iGlowIndex);
				memory.Write(glowObjectManager + (glowIndex * 0x38) + 0x8, globals::glowColorTeam[0]);
				memory.Write(glowObjectManager + (glowIndex * 0x38) + 0xC, globals::glowColorTeam[1]);
				memory.Write(glowObjectManager + (glowIndex * 0x38) + 0x10, globals::glowColorTeam[2]);
				memory.Write(glowObjectManager + (glowIndex * 0x38) + 0x14, globals::glowColorTeam[3]);

				memory.Write<bool>(glowObjectManager + (glowIndex * 0x38) + 0x27, true);
				memory.Write<bool>(glowObjectManager + (glowIndex * 0x38) + 0x28, true);
			}
			else if (globals::glowEnemy)
			{
				const auto& glowIndex = memory.Read<std::int32_t>(entity + hazedumper::netvars::m_iGlowIndex);
				memory.Write(glowObjectManager + (glowIndex * 0x38) + 0x8, globals::glowColorEnemy[0]);
				memory.Write(glowObjectManager + (glowIndex * 0x38) + 0xC, globals::glowColorEnemy[1]);
				memory.Write(glowObjectManager + (glowIndex * 0x38) + 0x10, globals::glowColorEnemy[2]);
				memory.Write(glowObjectManager + (glowIndex * 0x38) + 0x14, globals::glowColorEnemy[3]);

				memory.Write<bool>(glowObjectManager + (glowIndex * 0x38) + 0x27, true);
				memory.Write<bool>(glowObjectManager + (glowIndex * 0x38) + 0x28, true);
			}

			/*if (globals::chamsColorTeam && entityTeam == localTeam)
			{
				hacks::Color teamColor{ globals::glowColorTeam[0], globals::glowColorTeam[1] , globals::glowColorTeam[2] };
				memory.Write<hacks::Color>(entity + hazedumper::netvars::m_clrRender, teamColor);
				float brightness = 25.f;
				const auto _this = static_cast<std::uintptr_t>(globals::engineAdress + hazedumper::signatures::model_ambient_min - 0x2c);
				memory.Write<std::int32_t>(globals::engineAdress + hazedumper::signatures::model_ambient_min, *reinterpret_cast<std::uintptr_t*>(&brightness) ^ _this);
			}
			else if (globals::chamsEnemy)
			{
				hacks::Color enemyColor{ globals::glowColorEnemy[0], globals::glowColorEnemy[1] , globals::glowColorEnemy[2] };
				memory.Write<hacks::Color>(entity + hazedumper::netvars::m_clrRender, enemyColor);
				float brightness = 25.f;
				const auto _this = static_cast<std::uintptr_t>(globals::engineAdress + hazedumper::signatures::model_ambient_min - 0x2c);
				memory.Write<std::int32_t>(globals::engineAdress + hazedumper::signatures::model_ambient_min, *reinterpret_cast<std::uintptr_t*>(&brightness) ^ _this);
			}*/

			if (globals::radar && entityTeam != localTeam)
			{
				memory.Write(entity + hazedumper::netvars::m_bSpotted, true);
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void hacks::PlayerThread(const Memory& memory) noexcept
{
	while (gui::isRunning)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		const auto localPlayer = memory.Read<std::uintptr_t>(globals::clientAdress + hazedumper::signatures::dwLocalPlayer);
		if (!localPlayer) continue;

		if (globals::bunnyhop)
		{
			const auto onGround = memory.Read<bool>(localPlayer + hazedumper::netvars::m_fFlags);
			if (GetAsyncKeyState(VK_SPACE) && onGround & (1 << 0))
			{
				memory.Write<BYTE>(globals::clientAdress + hazedumper::signatures::dwForceJump, 6);
			}
		}

		if (globals::triggerbot)
		{
			const auto lifeState = memory.Read<std::int32_t>(localPlayer + hazedumper::netvars::m_lifeState);
			if (lifeState != 0) continue;

			const auto crosshairid = memory.Read<std::int32_t>(localPlayer + hazedumper::netvars::m_iCrosshairId);
			if (!crosshairid || crosshairid > 64) continue;

			const auto entity = memory.Read<std::uintptr_t>(globals::clientAdress + hazedumper::signatures::dwEntityList + (crosshairid - 1) * 0x10);
			if (memory.Read<std::int32_t>(entity + hazedumper::netvars::m_lifeState) != 0) continue;

			if (memory.Read<std::int32_t>(localPlayer + hazedumper::netvars::m_iTeamNum) == memory.Read<std::int32_t>(entity + hazedumper::netvars::m_iTeamNum)) continue;

			memory.Write<std::uintptr_t>(globals::clientAdress + hazedumper::signatures::dwForceAttack, 6);
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
			memory.Write<std::uintptr_t>(globals::clientAdress + hazedumper::signatures::dwForceAttack, 4);
		}

		/*if (globals::skinchanger)
		{
			const auto weapons = memory.Read<std::array<unsigned long, 8>>(localPlayer + hazedumper::netvars::m_hMyWeapons);

			for (const auto handle : weapons)
			{
				const auto weapon = memory.Read<std::uintptr_t>((globals::clientAdress + hazedumper::signatures::dwEntityList + (handle & 0xFFF) * 0x10));

				if (!weapon) continue;

				if (const auto paint = hacks::GetWeapon(memory.Read<short>(weapon + hazedumper::netvars::m_iItemDefinitionIndex)))
				{
					const bool shouldUpdate = memory.Read<std::int32_t>(weapon + hazedumper::netvars::m_nFallbackPaintKit) != paint;

					memory.Write<std::int32_t>(weapon + hazedumper::netvars::m_iItemIDHigh, -1);
					memory.Write<std::int32_t>(weapon + hazedumper::netvars::m_nFallbackPaintKit, paint);
					memory.Write<float>(weapon + hazedumper::netvars::m_flFallbackWear, 0.1f);

					if (shouldUpdate)
					{
						memory.Write<std::int32_t>(memory.Read<std::uintptr_t>(globals::engineAdress + hazedumper::signatures::dwClientState) + 0x174, -1);
					}
				}
			}
		}*/

		if (globals::aimbot)
		{
			const auto localEyePosition = memory.Read<Vector3>(localPlayer + hazedumper::netvars::m_vecOrigin) +
				memory.Read<Vector3>(localPlayer + hazedumper::netvars::m_vecViewOffset);

			const auto clientState = memory.Read<std::uintptr_t>(globals::engineAdress + hazedumper::signatures::dwClientState);
			const auto viewAngles = memory.Read< Vector3>(clientState + hazedumper::signatures::dwClientState_ViewAngles);
			const auto aimPunch = memory.Read<Vector3>(localPlayer + hazedumper::netvars::m_aimPunchAngle) * 2;

			auto bestFov = 5.f;
			auto bestAngle = Vector3{ };

			const auto localTeam = memory.Read<std::int32_t>(localPlayer + hazedumper::netvars::m_iTeamNum);

			for (auto i = 1; i <= 32; ++i)
			{
				const auto entity = memory.Read<std::uintptr_t>(globals::clientAdress + hazedumper::signatures::dwEntityList + i * 0x10);
				if (!entity) continue;
				if (entity == localPlayer) continue;

				const auto lifeState = memory.Read<std::int32_t>(entity + hazedumper::netvars::m_lifeState);
				if (lifeState != 0) continue;
				const auto entityTeam = memory.Read<std::int32_t>(entity + hazedumper::netvars::m_iTeamNum);

				if (entityTeam == localTeam) continue;

				if (memory.Read<bool>(entity + hazedumper::signatures::m_bDormant)) continue;
				if (!memory.Read<bool>(entity + hazedumper::netvars::m_bSpottedByMask)) continue;

				const auto boneMatrix = memory.Read<std::uintptr_t>(entity + hazedumper::netvars::m_dwBoneMatrix);

				const auto playerHeadPosition = Vector3{
					memory.Read<float>(boneMatrix + 0x30 * 8 + 0x0C), memory.Read<float>(boneMatrix + 0x30 * 8 + 0x1C), memory.Read<float>(boneMatrix + 0x30 * 8 + 0x2C) };

				const auto angle = CalculateAngle(localEyePosition, playerHeadPosition, viewAngles + aimPunch);

				const auto fov = std::hypot(angle.x, angle.y);

				if (fov < bestFov)
				{
					bestFov = fov;
					bestAngle = angle;
				}
			}

			if (!bestAngle.IsZero())
			{
				memory.Write<Vector3>(clientState + hazedumper::signatures::dwClientState_ViewAngles, viewAngles + bestAngle / 3.f);
			}

		}
	}
}

