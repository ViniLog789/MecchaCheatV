#include "autodisshadow.h"

using namespace MecchaCheatV::Features::Player;

AutoDissShadow::AutoDissShadow() : FeatureCore("Auto Disable Shadow", TYPE_PLAYER) {}

void AutoDissShadow::OnMenuRender()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));
	bool enabled = IsActive();
	if (ImGui::Checkbox("Enable auto disable shadow", &enabled))
	{
		SET_CONFIG_VALUE(GetConfigManager(), "Enabled", bool, enabled);
		if (enabled) OnActivate();
		else OnDeactivate();
	}
	ImGui::PopStyleVar();
}

void AutoDissShadow::AutoDissShadowHandle(SDK::UObject* Object)
{
	if (!IsActive())
		return;
	if (!Object)
		return;

	auto character = static_cast<SDK::ABP_FirstPersonCharacter_cLeon_Character_C*>(Object);
	if (!character)
		return;

	auto fn = character->Class->GetFunction("BP_FirstPersonCharacter_cLeon_Character_C", "SetCastShadow");
	if (fn && character->BodyShadow)
	{
		struct { bool BodyShadow_0; } params;
		params.BodyShadow_0 = false;
		character->ProcessEvent(fn, &params);
		NOTIFY_INFO_QUICK("Shadows disabled by MecchaCheatV");
	}
}