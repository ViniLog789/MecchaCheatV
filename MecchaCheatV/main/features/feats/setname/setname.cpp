#include "setname.h"

using namespace MecchaCheatV::Features::Player;

SetName::SetName() : FeatureCore("Set Name", TYPE_PLAYER)
{
	DECLARE_CONFIG(GetConfigManager(), "PlayerName", std::string, "");
}

void SetName::OnActivate()
{
	std::string playerName = CONFIG_STRING(GetConfigManager(), "PlayerName");
	if (!playerName.empty())
		SetPlayerName(playerName);
}

void SetName::OnDeactivate()
{
	if (!origName.empty())
		SetPlayerName(origName);
}

void SetName::OnMenuRender()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));
	bool enabled = IsActive();
	if (ImGui::Checkbox("Enable Set Name", &enabled))
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
	std::string playerName = CONFIG_STRING(GetConfigManager(), "PlayerName");

	char buffer[256];
	strncpy_s(buffer, playerName.c_str(), sizeof(buffer));
	if (ImGui::InputText("##player_name", buffer, sizeof(buffer)))
	{
		playerName = buffer;
		SET_CONFIG_VALUE(GetConfigManager(), "PlayerName", std::string, playerName);
	}

	ImGui::SameLine();

	if (ImGui::Button("Apply"))
	{
		if (!nameChanged)
			origName = GetPlayerName();
		nameChanged = true;
		SetPlayerName(playerName);
	}

	ImGui::PopStyleVar();
}