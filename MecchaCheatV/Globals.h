#pragma once
#include <atomic>
#include <Windows.h>
#include <string>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui_impl_win32.h"
#include "main/sdk/SDK.hpp"
using ProcessEvent_t = void(__fastcall*)(SDK::UObject*, SDK::UFunction*, void*);

namespace MecchaCheatV::Globals
{
	// Debug and Tests
	inline bool IsDebugging = false;
	inline bool IsCalledLogs = false;
	inline bool IsUpdateCalledLogs = false;

	// Main
	inline HMODULE globalModule{};
	inline std::atomic<bool> CheatWork{ false };
	inline int MenuToggleKey = VK_RETURN; // Enter
	inline std::string Version = "1.0";
	inline const char* GitHubRepository = "https://github.com/ViniLog789/MecchaCheatV";
	inline ProcessEvent_t hookedProcessEvent = nullptr;
	inline bool needTeleport = false;
	inline SDK::FVector teleportLocation = SDK::FVector(0, 0, 0);

	// ImGui
	inline float dpiScale = GetDpiForSystem() / 96.f;
	inline constexpr ImGuiWindowFlags WINDOW_FLAGS_GLOBALS =
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoResize;
	inline ImVec4 accentPurple = ImVec4(0.51f, 0.25f, 0.96f, 1.00f);
	inline ImVec4 darkerBg = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
	inline ImVec4 accentPurpleDark = ImVec4(0.41f, 0.15f, 0.86f, 1.00f);
	inline ImVec4 accentPurpleLight = ImVec4(0.61f, 0.35f, 1.00f, 1.00f);
	inline ImVec4 darkBg = ImVec4(0.08f, 0.08f, 0.08f, 0.98f);
	inline ImVec4 cardBg = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
	inline ImVec4 headerBg = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);

	// Functions
	inline void ResetColors()
	{
		accentPurple = ImVec4(0.51f, 0.25f, 0.96f, 1.00f);
		darkerBg = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		accentPurpleDark = ImVec4(0.41f, 0.15f, 0.86f, 1.00f);
		accentPurpleLight = ImVec4(0.61f, 0.35f, 1.00f, 1.00f);
		darkBg = ImVec4(0.08f, 0.08f, 0.08f, 0.98f);
		cardBg = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
		headerBg = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
	}
}