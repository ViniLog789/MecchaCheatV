#include "sprintmultiplier.h"
using namespace MecchaCheatV::Features::Player;

SprintMultiplier::SprintMultiplier() : FeatureCore("Sprint Multiplier", TYPE_PLAYER)
{
    DECLARE_CONFIG(GetConfigManager(), "SprintMultiplier", float, 2.f);
}

void SprintMultiplier::OnMenuRender()
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));
    
    ImGui::BeginHostOnly();

    bool enabled = IsActive();
    if (ImGui::Checkbox("Enable sprint multiplier", &enabled))
    {
        SET_CONFIG_VALUE(GetConfigManager(), "Enabled", bool, enabled);
        if (enabled) OnActivate();
        else OnDeactivate();
    }

    if (!enabled)
    {
        ImGui::EndHostOnly();
        ImGui::PopStyleVar();
        return;
    }

    float sprintMultiplier = GET_CONFIG_VALUE(GetConfigManager(), "SprintMultiplier", float);
    if (ImGui::SliderFloat("Multiplier", &sprintMultiplier, 1.f, 10.f, "%.2f"))
    {
        SET_CONFIG_VALUE(GetConfigManager(), "SprintMultiplier", float, sprintMultiplier);
    }

    ImGui::EndHostOnly();

    ImGui::PopStyleVar();
}

void SprintMultiplier::SprintMultiplierHandler()
{
    if (!IsActive() || SDK::UWorld::GetWorld() && !SDK::UKismetSystemLibrary::IsServer(SDK::UWorld::GetWorld())) return;

    static float lastAppliedMultiplier = 0.0f;
    static uint64_t lastUpdateFrame = 0;

    float sprintMultiplier = GET_CONFIG_VALUE(GetConfigManager(), "SprintMultiplier", float);
    if (sprintMultiplier < 1.0f)
        sprintMultiplier = 1.0f;

    auto pawn = reinterpret_cast<SDK::AMoverExamplesCharacter*>(Utils::GetAcknowledgedPawn());
    if (!pawn || !Utils::isObjectValid(pawn)) return;

    auto mover = pawn->GetMoverComponent();
    if (!mover || !Utils::isObjectValid(mover)) return;

    auto Settings = mover->FindSharedSettings_Mutable_BP(SDK::UCommonLegacyMovementSettings::StaticClass());
    if (!Settings) return;

    auto Legacy = static_cast<SDK::UCommonLegacyMovementSettings*>(Settings);
    if (!Legacy || !Utils::isObjectValid(Legacy)) return;

    uint64_t currentFrame = GetCurrentFrame();

    if (std::abs(lastAppliedMultiplier - sprintMultiplier) > 0.001f ||
        currentFrame > lastUpdateFrame + 3)
    {
        float needSpeed = 600.0f * sprintMultiplier;

        if (std::abs(Legacy->MaxSpeed - needSpeed) > 0.1f)
        {
            Legacy->MaxSpeed = needSpeed;
            lastAppliedMultiplier = sprintMultiplier;
            lastUpdateFrame = currentFrame;
        }
    }
}

uint64_t SprintMultiplier::GetCurrentFrame()
{
    return SDK::UKismetSystemLibrary::GetFrameCount();
}