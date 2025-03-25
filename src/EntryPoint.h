#pragma once
#include "imgui.h"

extern Application* CreateApplication(int argc, char** argv);
inline bool g_bRunning = true;

inline int Main(int argc, char** argv)
{
    while (g_bRunning)
    {
        Application* app = CreateApplication(argc, argv);
        app->Run();
        delete app;
    }
    return 0;
}

INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    return Main(__argc, __argv);
}

namespace Style
{
    inline void Theme()
    {
        auto& style = ImGui::GetStyle();
		ImVec4* colors = style.Colors;

		colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.282f, 0.2f, 0.522f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.282f, 0.2f, 0.522f, 0.50f);
		colors[ImGuiCol_TabActive] = ImVec4(0.282f, 0.2f, 0.522f, 0.50f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.282f, 0.2f, 0.522f, 0.50f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.282f, 0.2f, 0.522f, 0.50f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.282f, 0.2f, 0.522f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.282f, 0.2f, 0.522f, 1.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.282f, 0.2f, 0.522f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.482f, 0.3f, 0.622f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.4f, 0.306f, 0.686f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.282f, 0.2f, 0.522f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.282f, 0.2f, 0.522f, 0.50f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.282f, 0.2f, 0.522f, 1.00f);
		
		style.WindowRounding = 10.0f;
		style.FrameRounding = 5.0f;
		style.GrabRounding = 10.0f;
		style.PopupRounding = 10.0f;
		style.ScrollbarRounding = 10.0f;
		style.TabRounding = 10.0f;
		style.FramePadding.y = 5.0f;
		style.ButtonTextAlign = ImVec2(0.5, 0.5);
    }
}