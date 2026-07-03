#include "teleport.h"

using namespace MecchaCheatV::Features::Player;

Teleport::Teleport() : FeatureCore("Teleport", TYPE_PLAYER)
{
	DECLARE_CONFIG(GetConfigManager(), "TeleportLocationX", float, 0.0f);
	DECLARE_CONFIG(GetConfigManager(), "TeleportLocationY", float, 0.0f);
	DECLARE_CONFIG(GetConfigManager(), "TeleportLocationZ", float, 0.0f);
}

void Teleport::OnActivate()
{
	UpdatePlayers();
}

void Teleport::OnDeactivate()
{
	cachedPlayers.clear();
	selectedPlayerIndex = -1;
}

void Teleport::UpdatePlayers()
{
    cachedPlayers.clear();

    SDK::UWorld* world = SDK::UWorld::GetWorld();
    if (!world)
        return;

    SDK::AGameStateBase* gameState = world->GameState;
    if (!gameState)
        return;

    for (auto playerState : gameState->PlayerArray)
    {
        if (!playerState || !Utils::isObjectValid(playerState))
            continue;

        SDK::APawn* pawn = playerState->PawnPrivate;
        if (!pawn || !Utils::isObjectValid(pawn))
            continue;

        cachedPlayers.push_back(pawn);
    }
}

void Teleport::OnMenuRender()
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

    bool enabled = IsActive();
    if (ImGui::Checkbox("Enable Teleport", &enabled))
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

    ImGui::BeginHostOnly();

    ImGui::SeparatorText("Custom teleport");

    float teleportX = CONFIG_FLOAT(GetConfigManager(), "TeleportLocationX");
    if (ImGui::InputFloat("x", &teleportX))
        SET_CONFIG_VALUE(GetConfigManager(), "TeleportLocationX", float, teleportX);

    float teleportY = CONFIG_FLOAT(GetConfigManager(), "TeleportLocationY");
    if (ImGui::InputFloat("y", &teleportY))
        SET_CONFIG_VALUE(GetConfigManager(), "TeleportLocationY", float, teleportY);

    float teleportZ = CONFIG_FLOAT(GetConfigManager(), "TeleportLocationZ");
    if (ImGui::InputFloat("z", &teleportZ))
        SET_CONFIG_VALUE(GetConfigManager(), "TeleportLocationZ", float, teleportZ);

    if (ImGui::Button("Teleport to coords"))
    {
        SDK::FVector loc(
            teleportX,
            teleportY,
            teleportZ
        );

        Utils::RequestTeleport(loc);
    }

    ImGui::SeparatorText("Teleport to player");

    if (ImGui::Button("Refresh players"))
    {
        UpdatePlayers();
        selectedPlayerIndex = -1;
    }

    if (!cachedPlayers.empty())
    {
        std::vector<std::string> playerNames;
        std::vector<const char*> names;

        playerNames.reserve(cachedPlayers.size());
        names.reserve(cachedPlayers.size());

        for (auto* pawn : cachedPlayers)
        {
            if (!Utils::isObjectValid(pawn))
            {
                playerNames.emplace_back("Invalid Player");
                continue;
            }

            SDK::APlayerState* ps = pawn->PlayerState;
            if (ps)
                playerNames.emplace_back(ps->PlayerNamePrivate.ToString());
            else
                playerNames.emplace_back("Unknown");
        }

        for (auto& str : playerNames)
            names.push_back(str.c_str());

        ImGui::Combo("Select player", &selectedPlayerIndex, names.data(), (int)names.size());
    }
    else
    {
        ImGui::Text("No players found");
    }

    if (ImGui::Button("Teleport to player"))
    {
        if (selectedPlayerIndex >= 0 && selectedPlayerIndex < cachedPlayers.size())
        {
            SDK::APawn* target = cachedPlayers[selectedPlayerIndex];
            if (Utils::isObjectValid(target))
            {
                Utils::RequestTeleportToActor(target);
            }
        }
    }

	ImGui::EndHostOnly();

    ImGui::PopStyleVar();
}