#include "nodetection.h"

using namespace MecchaCheatV::Features::Player;

NoDetection::NoDetection() : FeatureCore("No Detection", TYPE_PLAYER) {}

void NoDetection::OnMenuRender()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

	bool enabled = IsActive();
	if (ImGui::Checkbox("Enable no detection (disable too burning)", &enabled))
	{
		SET_CONFIG_VALUE(GetConfigManager(), "Enabled", bool, enabled);
		if (enabled) OnActivate();
		else OnDeactivate();
	}

	ImGui::PopStyleVar();
}

void NoDetection::NoDetectionHandler()
{
	if (!IsActive())
		return;

	auto localPawn = Utils::GetAcknowledgedPawn();
	if (!localPawn || !Utils::isObjectValid(localPawn) || !Utils::isSurvivor(localPawn))
		return;

	auto survivor = static_cast<SDK::ABP_FirstPersonCharacter_cLeon_Character_Survivor_C*>(localPawn);
	survivor->OverlapCheckCapsules.Clear();
}