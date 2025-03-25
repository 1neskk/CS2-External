#include <numbers>
#include <array>

#include "Cheat.h"
#include "imgui.h"

Cheat::~Cheat()
{
	m_dwProcessId = 0;
	m_baseAddress = 0;
	m_localPlayerData.entityPawn = 0;
	m_entityList = 0;

#ifdef _DEBUG
	FreeConsole();
#endif
}

auto Cheat::Attach(const WCHAR* processName) -> void
{
#ifdef _DEBUG
	AllocConsole();
	freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
	freopen_s(reinterpret_cast<FILE**>(stderr), "CONOUT$", "w", stderr);
#endif

	m_dwProcessId = Memory::GetProcessId(processName);
	if (!m_dwProcessId)
	{
		std::cerr << "[-] Process not found\n";
		return;
	}

	m_baseAddress = Driver::GetModuleBaseAddress(m_dwProcessId, "client.dll");
	if (!m_baseAddress)
	{
		std::cerr << "[-] Failed to get base address\n";
		return;
	}

	if (IsAttached())
	{
		std::cout << "[+] Attached to process with PID: " << m_dwProcessId << "\n";
		std::cout << "[+] Base address: 0x" << std::hex << m_baseAddress << std::dec << "\n";
	}
	else
		std::cerr << "[-] Failed to attach to process\n";
}

bool Cheat::IsAttached() const
{
	return m_dwProcessId && m_baseAddress;
}

void Cheat::OnUpdate()
{
	if (GetAsyncKeyState(VK_END) & 1)
		Detach();

	if (!m_localPlayerData.entityPawn)
	{
		m_localPlayerData.entityPawn = Driver::Read<uintptr_t>(m_dwProcessId, m_baseAddress + cs2_dumper::offsets::client_dll::dwLocalPlayerPawn);
	}

	if (!m_localPlayerData.entityPawn)
		return;

	if (!m_entityList)
	{
		m_entityList = Driver::Read<uintptr_t>(m_dwProcessId, m_baseAddress + cs2_dumper::offsets::client_dll::dwEntityList);
	}

	if (!m_entityList)
		return;

	m_width = static_cast<float>(GetSystemMetrics(SM_CXSCREEN));
	m_height = static_cast<float>(GetSystemMetrics(SM_CYSCREEN));

	m_viewMatrix = Driver::Read<glm::mat4>(m_dwProcessId, m_baseAddress + cs2_dumper::offsets::client_dll::dwViewMatrix);
	m_localPlayerData.team = Driver::Read<int>(m_dwProcessId, m_localPlayerData.entityPawn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iTeamNum);
	m_localPlayerData.origin = Driver::Read<glm::vec3>(m_dwProcessId, m_localPlayerData.entityPawn + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_vOldOrigin);

	m_entities = GetEntityData();

	if (m_bBunnyHop)
		BunnyHop();

	if (m_bAimbot)
		Aimbot(m_entities);
}

void Cheat::BunnyHop() const
{
	const auto flags = Driver::Read<uint32_t>(m_dwProcessId, m_localPlayerData.entityPawn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_fFlags);

	const bool bAir = flags & (1 << 0);
	const bool bSpacePressed = GetAsyncKeyState(VK_SPACE);
	const auto forceJump = Driver::Read<DWORD>(m_dwProcessId, m_baseAddress + cs2_dumper::buttons::jump);

	if (bSpacePressed && bAir)
	{
		Sleep(1);
		Driver::Write<DWORD>(m_dwProcessId, m_baseAddress + cs2_dumper::buttons::jump, 65537);
	}
	else if (bSpacePressed && !bAir)
		Driver::Write<DWORD>(m_dwProcessId, m_baseAddress + cs2_dumper::buttons::jump, 256);
	else if (!bSpacePressed && forceJump == 65537)
		Driver::Write<DWORD>(m_dwProcessId, m_baseAddress + cs2_dumper::buttons::jump, 256);
}

void Cheat::Aimbot(const std::vector<EntityData>& entities) const
{
	uintptr_t nearestEntity = ComputeNearest(m_localPlayerData.origin, entities);
	const auto nearestOrigin = Driver::Read<glm::vec3>(m_dwProcessId, nearestEntity + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_vOldOrigin);

	const float dx = nearestOrigin.x - m_localPlayerData.origin.x;
	const float dy = nearestOrigin.y - m_localPlayerData.origin.y;
	const float dz = nearestOrigin.z - m_localPlayerData.origin.z;
	const float distance = sqrtf(dx * dx + dy * dy + dz * dz);

	glm::vec2 newAngles;
	newAngles.x = -asinf(dz / distance) * (180.0f / static_cast<float>(std::numbers::pi));
	newAngles.y = atan2f(dy, dx) * (180.0f / static_cast<float>(std::numbers::pi));

	if (GetAsyncKeyState(VK_RBUTTON))
	{
		Driver::Write(m_dwProcessId, m_baseAddress + cs2_dumper::offsets::client_dll::dwViewAngles, newAngles);
	}
}

uintptr_t Cheat::ComputeNearest(glm::vec3 localOrigin, const std::vector<EntityData>& entities) const
{
	float distance = FLT_MAX;
	uintptr_t nearestEntity = 0;

	for (const auto& entity : entities)
	{
		if (entity.team == m_localPlayerData.team || entity.origin == localOrigin)
			continue;

		const float dx = entity.origin.x - localOrigin.x;
		const float dy = entity.origin.y - localOrigin.y;
		const float dz = entity.origin.z - localOrigin.z;
		float sqrDistance = dx * dx + dy * dy + dz * dz;

		if (sqrDistance < distance)
		{
			distance = sqrDistance;
			nearestEntity = entity.entityPawn;
		}
	}
	return nearestEntity;
}

void Cheat::RenderEsp(const std::vector<EntityData>& entities) const
{
	for (const auto& entity : entities)
	{
		const glm::vec3 head = { entity.origin.x, entity.origin.y, entity.origin.z + 72.0f };

		glm::vec3 screenHead = head;
		screenHead.z += 10.0f;

		glm::vec3 screenOrigin = entity.origin;
		screenOrigin.z += 10.0f;

		if (WorldToScreen(head, screenHead, m_viewMatrix) && WorldToScreen(entity.origin, screenOrigin, m_viewMatrix))
		{
			const auto distance = glm::distance(m_localPlayerData.origin, entity.origin) / 10.0f;
			const auto healthPercentage = static_cast<float>(entity.health) / 100.f;

			const auto healthColor = IM_COL32(255 * (1 - healthPercentage), 255 * healthPercentage, 0, 255);
			constexpr auto colorBackground = glm::vec4(0.f, 0.f, 0.f, 1.0f);

			const auto width = (screenOrigin.y - screenHead.y) / 2;
			const auto height = screenOrigin.y - screenHead.y;

			ImGui::GetBackgroundDrawList()->AddRect(ImVec2(screenHead.x - width / 2, screenHead.y), ImVec2(screenHead.x + width / 2, screenHead.y + height),
				ImColor(colorBackground.x, colorBackground.y, colorBackground.z, colorBackground.w), 0.f, 0, 1.f);

			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(screenHead.x - width / 2, screenOrigin.y), ImVec2(screenHead.x - width / 2 + 5,
				screenOrigin.y - height * healthPercentage), ImColor(healthColor), 0.f, 0);
			ImGui::GetBackgroundDrawList()->AddRect(ImVec2(screenHead.x - width / 2, screenHead.y), ImVec2(screenHead.x - width / 2 + 5, screenHead.y + height),
				ImColor(0, 0, 0, 255), 0.f, 0, 1.f);

			ImGui::GetBackgroundDrawList()->AddText(ImVec2(screenHead.x - width / 2, screenHead.y - 15.0f), ImColor(255, 255, 255, 255), entity.name.c_str());

			if (distance < 1000.f)
			{
				const auto distanceText = std::format("{:.2f}m", distance);
				ImGui::GetBackgroundDrawList()->AddText(ImVec2(screenHead.x - width / 2, screenHead.y + height), ImColor(255, 255, 255, 255), distanceText.c_str());
			}
		}
	}
}

std::vector<EntityData> Cheat::GetEntityData() const
{
	std::vector<EntityData> entities;

	for (int i = 0; i < 32; i++)
	{
		const auto listEntry1 = Driver::Read<uintptr_t>(m_dwProcessId, m_entityList + (8 * (i & 0x7FFF) >> 9) + 16);
		if (!listEntry1)
			continue;

		const auto entityController = Driver::Read<uintptr_t>(m_dwProcessId, listEntry1 + 120 * (i & 0x1FF));
		if (!entityController)
			continue;

		const auto entityControllerPawn = Driver::Read<uint32_t>(m_dwProcessId, entityController + cs2_dumper::schemas::client_dll::CBasePlayerController::m_hPawn);
		if (!entityControllerPawn)
			continue;

		const auto listEntry2 = Driver::Read<uintptr_t>(m_dwProcessId, m_entityList + 0x8 * ((entityControllerPawn & 0x7FFF) >> 9) + 16);
		if (!listEntry2)
			continue;

		const auto entityPawn = Driver::Read<uintptr_t>(m_dwProcessId, listEntry2 + 120 * (entityControllerPawn & 0x1FF));
		if (!entityPawn)
			continue;

		const auto entityTeam = Driver::Read<int>(m_dwProcessId, entityPawn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iTeamNum);
		const auto health = Driver::Read<int>(m_dwProcessId, entityPawn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iHealth);
		if (health <= 0 || health > 100)
			continue;

		if (entityPawn == m_localPlayerData.entityPawn)
			continue;

		const auto origin = Driver::Read<glm::vec3>(m_dwProcessId, entityPawn + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_vOldOrigin);
		auto buffer = Driver::Read<std::array<char, 128>>(m_dwProcessId, entityController + cs2_dumper::schemas::client_dll::CBasePlayerController::m_iszPlayerName);
		const auto name = buffer.data();

		if (!m_bTeamEsp && (entityTeam == m_localPlayerData.team))
			continue;

		EntityData data = {};
		data.entityPawn = entityPawn;
		data.entityController = entityController;
		data.team = entityTeam;
		data.health = health;
		data.origin = origin;
		data.name = name;

		entities.push_back(data);
	}
	return entities;
}

bool Cheat::WorldToScreen(const glm::vec3& pos, glm::vec3& screen, const glm::mat4& viewMatrix) const
{
	const auto w = viewMatrix[3].x * pos.x + viewMatrix[3].y * pos.y + viewMatrix[3].z * pos.z + viewMatrix[3].w;
	if (w < 0.001f)
		return false;
	const auto x = viewMatrix[0].x * pos.x + viewMatrix[0].y * pos.y + viewMatrix[0].z * pos.z + viewMatrix[0].w;
	const auto y = viewMatrix[1].x * pos.x + viewMatrix[1].y * pos.y + viewMatrix[1].z * pos.z + viewMatrix[1].w;
	screen.x = (m_width / 2.0f) * (1.0f + x / w);
	screen.y = (m_height / 2.0f) * (1.0f - y / w);
	screen.z = w;
	return true;
}
