#include "Application.h"
#include "EntryPoint.h"
#include "Drawing.h"
#include "Cheat.h"

class CS2 final : public Layer
{
public:
	void OnUpdate(const float ts) override
    {
		if (m_cheat.IsAttached())
		{
			m_cheat.OnUpdate();
		}

		if (GetAsyncKeyState(VK_END) & 1)
		{
			Application::Close();
		}

		if (GetAsyncKeyState(VK_INSERT) & 1)
		{
			if (Drawing::IsActive())
				Drawing::SetActive(false);
			else
				Drawing::SetActive(true);
		}
    }

	void OnImGuiRender() override
    {
		char fpsBuffer[16] = { 0 };
		std::snprintf(fpsBuffer, sizeof(fpsBuffer), "FPS: %.2f", ImGui::GetIO().Framerate);
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 100, 15.0f));
		ImGui::SetNextWindowSize(ImVec2(0.f, 0.f));

		ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::Text(fpsBuffer);
		ImGui::End();

		if (Drawing::IsActive())
		{
			ImGuiWindowFlags windowFlagsMenu = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
			ImGuiWindowFlags windowFlagsLoader = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;

			Style::Theme();

			ImGui::SetNextWindowSize(ImVec2(400, 250), ImGuiCond_Once);
			ImGui::SetNextWindowBgAlpha(0.80f);
			if (m_cheat.IsAttached())
			{
				ImGui::Begin("Nesk External", nullptr, windowFlagsMenu);
				ImGui::Text("Process ID: %d", m_cheat.GetProcessId());
				ImGui::Text("Base Address: 0x%X", m_cheat.GetBaseAddress());

				ImGui::Separator();
				ImGui::Checkbox("Toggle BunnyHop", &m_cheat.m_bBunnyHop);
				ImGui::Checkbox("Toggle ESP", &m_cheat.m_bEsp);

				if (m_cheat.m_bEsp)
				{
					ImGui::Checkbox("Team ESP", &m_cheat.m_bTeamEsp);
				}

				ImGui::Checkbox("Toggle Aimbot", &m_cheat.m_bAimbot);

				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 10.0f));
				if (ImGui::Button("Detach"))
				{
					m_cheat.Detach();
					Application::Close();
				}
				ImGui::End();
			}
			else
			{
				ImGui::Begin("Nesk External Loader", nullptr, windowFlagsLoader);
				ImGui::Text("Attach to: cs2.exe");
				if (ImGui::Button("Attach"))
					m_cheat.Attach(L"cs2.exe");

				ImGui::SameLine();
				ImGui::SetCursorPosX(90.0f);

				if (ImGui::Button("Exit"))
					Application::Close();
				ImGui::End();
			}
		}

		if (m_cheat.IsAttached() && m_cheat.ToggleEsp())
			m_cheat.RenderEsp(m_cheat.GetEntities());
    }

private:
    Cheat m_cheat;
};

Application* CreateApplication(int argc, char** argv)
{
    Specs spec;
    spec.title = "CS2 External";

    auto app = new Application(spec);
    app->PushLayer<CS2>();

    return app;
}

