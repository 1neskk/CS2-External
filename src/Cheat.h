#pragma once

#include <iostream>
#include <format>
#include <string>
#include <glm/glm.hpp>

#include "Driver.h"
#include "offsets/client_dll.hpp"
#include "offsets/offsets.hpp"
#include "offsets/buttons.hpp"

struct EntityData
{
    uintptr_t entityPawn;
	uintptr_t entityController;
    int team;
	int health;
	glm::vec3 origin;
	std::string name;
};

struct LocalPlayerData
{
	uintptr_t entityPawn;
	int team;
	glm::vec3 origin;
};

class Cheat
{
public:
    Cheat() = default;
    ~Cheat();

    auto Attach(const WCHAR* processName) -> void;
    void Detach() { this->~Cheat(); }
    bool IsAttached() const;

    void OnUpdate();

    void Aimbot(const std::vector<EntityData>& entities) const;
    uintptr_t ComputeNearest(glm::vec3 localOrigin, const std::vector<EntityData>& entities) const;
	void BunnyHop() const;

    bool WorldToScreen(const glm::vec3& pos, glm::vec3& screen, const glm::mat4& viewMatrix) const;

    void RenderEsp(const std::vector<EntityData>& entities) const;
    bool ToggleEsp() const { return m_bEsp; }
    bool ToggleTeamEsp() const { return m_bTeamEsp; }


    DWORD GetProcessId() const { return m_dwProcessId; }
    uintptr_t GetBaseAddress() const { return m_baseAddress; }

	std::vector<EntityData> GetEntities() const { return m_entities; }
private:
    std::vector<EntityData> GetEntityData() const;

public:
    bool m_bAimbot = false;
    bool m_bEsp = false;
    bool m_bTeamEsp = false;
    bool m_bBunnyHop = false;

private:
    float m_width, m_height;
    DWORD m_dwProcessId = 0;
    uintptr_t m_baseAddress = 0;

    LocalPlayerData m_localPlayerData;
    uintptr_t m_entityList = 0;
    glm::mat4 m_viewMatrix = NULL;

	std::vector<EntityData> m_entities;
};

