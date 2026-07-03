#include "alwvisible.h"

using namespace MecchaCheatV::Features::Player;

AlwaysVisible::AlwaysVisible() : FeatureCore("Always Visible", TYPE_PLAYER) {}

void AlwaysVisible::OnMenuRender()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

	bool enabled = IsActive();
	if (ImGui::Checkbox("Enable always visible (Infection)", &enabled))
	{
		SET_CONFIG_VALUE(GetConfigManager(), "Enabled", bool, enabled);
		if (enabled) OnActivate();
		else OnDeactivate();
	}

	ImGui::PopStyleVar();
}

void AlwaysVisible::AlwaysVisibleHandle(SDK::UObject* Object)
{
	if (!IsActive())
		return;
	if (!Object)
		return;
	auto character = static_cast<SDK::ABP_FirstPersonCharacter_cLeon_Character_C*>(Object);
	character->BodyVisibility = true;
}