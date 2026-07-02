#include "fovchanger.h"

using namespace MecchaCheatV::Features::Player;

FOVChanger::FOVChanger() : FeatureCore("FOV Changer", TYPE_PLAYER)
{
	DECLARE_CONFIG(GetConfigManager(), "FOVValue", float, 90.0f);
}

void FOVChanger::OnActivate()
{
	float fovValue = CONFIG_FLOAT(GetConfigManager(), "FOVValue");
	if (!SetFOV(fovValue)) NOTIFY_ERROR_QUICK("FoV change failed");
}

void FOVChanger::OnDeactivate()
{
	resetDefault();
}

void FOVChanger::OnMenuRender()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

	bool enabled = IsActive();
	if (ImGui::Checkbox("Enable FoV changer", &enabled))
	{
		SET_CONFIG_VALUE(GetConfigManager(), "Enabled", bool, enabled);
		if (enabled) OnActivate();
		else OnDeactivate();
	}

	if (!enabled)
	{
		ImGui::PopStyleVar();
		return;
	}

	float fovValue = CONFIG_FLOAT(GetConfigManager(), "FOVValue");
	if (ImGui::SliderFloat("FoV Value", &fovValue, 30.0f, 150.0f))
	{
		SET_CONFIG_VALUE(GetConfigManager(), "FOVValue", float, fovValue);
		if (!SetFOV(fovValue)) NOTIFY_ERROR_QUICK("FoV change failed");
	}

	if (ImGui::Button("Reset FoV"))
	{
		SET_CONFIG_VALUE(GetConfigManager(), "FOVValue", float, 90.f);
		if (resetDefault()) NOTIFY_SUCCESS_QUICK("FoV reset to default");
		else NOTIFY_ERROR_QUICK("FoV reset failed");
	}

	ImGui::PopStyleVar();
}