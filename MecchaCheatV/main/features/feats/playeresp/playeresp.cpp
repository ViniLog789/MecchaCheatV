// Some code is taken from phxgg.
#include "playeresp.h"
#include <algorithm>

using namespace MecchaCheatV::Features::Visuals;

static std::unordered_map<SDK::USkeletalMesh*, std::vector<std::pair<int32_t, int32_t>>> g_BonePairsCache;

PlayerESP::PlayerESP() : FeatureCore("Player ESP", TYPE_VISUALS)
{
    DECLARE_CONFIG(GetConfigManager(), "ShowName", bool, true);
    DECLARE_CONFIG(GetConfigManager(), "ShowDistance", bool, true);
    DECLARE_CONFIG(GetConfigManager(), "ShowRole", bool, true);
    DECLARE_CONFIG(GetConfigManager(), "ShowSnapline", bool, true);
    DECLARE_CONFIG(GetConfigManager(), "ShowDead", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "ShowOnlyHunters", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "ShowOnlySurvivors", bool, false);

    DECLARE_CONFIG(GetConfigManager(), "BoxEnabled", bool, true);
    DECLARE_CONFIG(GetConfigManager(), "BoxType", int, 0);
    DECLARE_CONFIG(GetConfigManager(), "BoxThickness", float, 2.0f);

    DECLARE_CONFIG(GetConfigManager(), "TextScale", float, 1.0f);
    DECLARE_CONFIG(GetConfigManager(), "ShowLocalPlayer", bool, false);

    DECLARE_CONFIG(GetConfigManager(), "ShowSkeleton", bool, true);
    DECLARE_CONFIG(GetConfigManager(), "SkeletonThickness", float, 1.5f);
    DECLARE_CONFIG(GetConfigManager(), "SkeletonColor", ImU32, IM_COL32(0, 255, 255, 255));

    DECLARE_CONFIG(GetConfigManager(), "NameColor", ImColor, ImColor(244, 244, 244, 255));
    DECLARE_CONFIG(GetConfigManager(), "SnaplineColor", ImColor, ImColor(0, 244, 0, 255));
    DECLARE_CONFIG(GetConfigManager(), "VisibleColor", ImColor, ImColor(0, 255, 0, 255));
    DECLARE_CONFIG(GetConfigManager(), "InvisibleColor", ImColor, ImColor(255, 0, 0, 255));
    DECLARE_CONFIG(GetConfigManager(), "HunterColor", ImColor, ImColor(255, 215, 0, 255));
    DECLARE_CONFIG(GetConfigManager(), "SurvivorColor", ImColor, ImColor(0, 222, 236, 255));
}

void PlayerESP::DrawSkeleton(SDK::USkeletalMeshComponent* Mesh, SDK::APlayerController* PC, ImDrawList* drawList, ImU32 color, float thickness)
{
    if (!Mesh || !Utils::isObjectValid(Mesh)) return;
    if (!PC || !Utils::isObjectValid(PC)) return;
    if (!drawList) return;

    auto skeletalMesh = Mesh->SkeletalMesh;
    if (!skeletalMesh || !Utils::isObjectValid(skeletalMesh)) return;

    int32_t numBones = Mesh->GetNumBones();
    if (numBones <= 0 || numBones > 500) return;

    static const std::pair<int, int> Bones[] =
    {
        {0,1},{1,2},{2,3},{3,4},{4,5},{5,6},
        {4,8},{8,9},{9,10},{10,11},
        {4,13},{13,14},{14,15},{15,16},
        {1,18},{18,19},{19,20},{20,21},
        {1,23},{23,24},{24,25},{25,26}
    };

    for (const auto& [parent, child] : Bones)
    {
        if (parent >= numBones || child >= numBones)
            continue;

        SDK::FName boneName1 = Mesh->GetBoneName(parent);
        SDK::FName boneName2 = Mesh->GetBoneName(child);

        if (boneName1.IsNone() || boneName2.IsNone())
            continue;

        SDK::FVector WorldLoc1 = Mesh->GetSocketLocation(boneName1);
        SDK::FVector WorldLoc2 = Mesh->GetSocketLocation(boneName2);

        SDK::FVector2D Screen1, Screen2;
        if (Utils::WorldToScreen(WorldLoc1, Screen1, false) && Utils::WorldToScreen(WorldLoc2, Screen2, false))
        {
            drawList->AddLine(
                ImVec2(Screen1.X, Screen1.Y),
                ImVec2(Screen2.X, Screen2.Y),
                color,
                thickness);
        }
    }
}

bool PlayerESP::DrawBox(SDK::AActor* actor, float& outMinX, float& outMinY, float& outMaxX, float& outMaxY)
{
    if (!actor || !Utils::isObjectValid(actor)) return false;

    SDK::FVector origin, boxExtent;
    actor->GetActorBounds(true, &origin, &boxExtent, false);

    if (boxExtent.X <= 0 || boxExtent.Y <= 0 || boxExtent.Z <= 0)
        return false;

    SDK::FVector corners[8] = {
        { origin.X - boxExtent.X, origin.Y - boxExtent.Y, origin.Z - boxExtent.Z },
        { origin.X + boxExtent.X, origin.Y - boxExtent.Y, origin.Z - boxExtent.Z },
        { origin.X - boxExtent.X, origin.Y + boxExtent.Y, origin.Z - boxExtent.Z },
        { origin.X + boxExtent.X, origin.Y + boxExtent.Y, origin.Z - boxExtent.Z },
        { origin.X - boxExtent.X, origin.Y - boxExtent.Y, origin.Z + boxExtent.Z },
        { origin.X + boxExtent.X, origin.Y - boxExtent.Y, origin.Z + boxExtent.Z },
        { origin.X - boxExtent.X, origin.Y + boxExtent.Y, origin.Z + boxExtent.Z },
        { origin.X + boxExtent.X, origin.Y + boxExtent.Y, origin.Z + boxExtent.Z }
    };

    SDK::FVector2D screen[8];
    bool valid = true;

    for (int i = 0; i < 8; ++i)
    {
        if (!Utils::WorldToScreen(corners[i], screen[i], false))
        {
            valid = false;
            break;
        }
    }

    if (!valid) return false;

    outMinX = outMaxX = screen[0].X;
    outMinY = outMaxY = screen[0].Y;

    for (int i = 1; i < 8; ++i)
    {
        outMinX = std::min<float>(outMinX, screen[i].X);
        outMinY = std::min<float>(outMinY, screen[i].Y);
        outMaxX = std::max<float>(outMaxX, screen[i].X);
        outMaxY = std::max<float>(outMaxY, screen[i].Y);
    }

    if (outMinX >= outMaxX || outMinY >= outMaxY)
        return false;

    return true;
}

ImU32 PlayerESP::GetPlayerColor(SDK::AActor* actor, bool isHunter, bool isVisible)
{
    if (isHunter)
        return CONFIG_COLOR(GetConfigManager(), "HunterColor");

    if (isVisible)
        return CONFIG_COLOR(GetConfigManager(), "VisibleColor");

    return CONFIG_COLOR(GetConfigManager(), "InvisibleColor");
}

void PlayerESP::DrawCornerBox(ImDrawList* drawList, float minX, float minY, float maxX, float maxY, ImU32 color, float thickness)
{
    float lineW = (maxX - minX) * 0.25f;
    float lineH = (maxY - minY) * 0.25f;

    ImU32 outlineColor = IM_COL32(0, 0, 0, 180);
    float outlineThickness = thickness + 2.0f;

    auto DrawLineWithOutline = [&](float x1, float y1, float x2, float y2)
        {
            drawList->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), outlineColor, outlineThickness);
            drawList->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), color, thickness);
        };

    DrawLineWithOutline(minX, minY, minX + lineW, minY);
    DrawLineWithOutline(minX, minY, minX, minY + lineH);
    DrawLineWithOutline(maxX, minY, maxX - lineW, minY);
    DrawLineWithOutline(maxX, minY, maxX, minY + lineH);
    DrawLineWithOutline(minX, maxY, minX + lineW, maxY);
    DrawLineWithOutline(minX, maxY, minX, maxY - lineH);
    DrawLineWithOutline(maxX, maxY, maxX - lineW, maxY);
    DrawLineWithOutline(maxX, maxY, maxX, maxY - lineH);
}

void PlayerESP::DrawFullBox(ImDrawList* drawList, float minX, float minY, float maxX, float maxY, ImU32 color, float thickness)
{
    ImU32 outlineColor = IM_COL32(0, 0, 0, 180);
    float outlineThickness = thickness + 2.0f;

    drawList->AddRect(ImVec2(minX, minY), ImVec2(maxX, maxY), outlineColor, 0.0f, 0, outlineThickness);
    drawList->AddRect(ImVec2(minX, minY), ImVec2(maxX, maxY), color, 0.0f, 0, thickness);
}

void PlayerESP::DrawRoundedBox(ImDrawList* drawList, float minX, float minY, float maxX, float maxY, ImU32 color, float thickness)
{
    float radius = (maxX - minX) * 0.08f;
    radius = std::clamp(radius, 2.0f, 8.0f);

    ImU32 outlineColor = IM_COL32(0, 0, 0, 180);
    float outlineThickness = thickness + 2.0f;

    drawList->AddRect(ImVec2(minX, minY), ImVec2(maxX, maxY), outlineColor, radius, 0, outlineThickness);
    drawList->AddRect(ImVec2(minX, minY), ImVec2(maxX, maxY), color, radius, 0, thickness);
}

void PlayerESP::DrawSnapline(ImDrawList* drawList, const SDK::FVector2D& target, ImU32 color, float thickness)
{
    ImVec2 screenCenter(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y);
    drawList->AddLine(screenCenter, ImVec2(target.X, target.Y), color, thickness);
}

std::string PlayerESP::GetRoleText(SDK::AActor* actor)
{
    if (!actor || !Utils::isObjectValid(actor)) return "";

    if (actor->IsA(SDK::ABP_FirstPersonCharacter_cLeon_Character_Hunter_C::StaticClass()))
        return "Hunter";

    if (actor->IsA(SDK::ABP_FirstPersonCharacter_cLeon_Character_Survivor_C::StaticClass()))
        return "Survivor";

    return "";
}

void PlayerESP::OnRender()
{
    auto world = SDK::UWorld::GetWorld();
    if (!world || !Utils::IsInMatch(world) || !Utils::isObjectValid(world)) return;

    auto GameState = Utils::GetGameStateBase();
    if (!GameState || !Utils::isObjectValid(GameState)) return;

    auto& playerArray = GameState->PlayerArray;
    if (!playerArray.IsValid()) return;

    SDK::APlayerController* localPC = nullptr;
    SDK::FVector localLocation{};
    bool hasLocalLocation = false;
    SDK::APawn* localPawn = nullptr;

    auto localPlayer = Utils::GetLocalPlayer();
    if (localPlayer && Utils::isObjectValid(localPlayer))
    {
        if (localPlayer->PlayerController && Utils::isObjectValid(localPlayer->PlayerController))
        {
            localPC = localPlayer->PlayerController;
            localPawn = localPC->AcknowledgedPawn;

            if (localPC->PlayerCameraManager && Utils::isObjectValid(localPC->PlayerCameraManager))
            {
                localLocation = localPC->PlayerCameraManager->GetCameraLocation();
                hasLocalLocation = true;
            }
            else if (localPawn && Utils::isObjectValid(localPawn))
            {
                localLocation = localPawn->K2_GetActorLocation();
                hasLocalLocation = true;
            }
        }
    }

    const bool showName = CONFIG_BOOL(GetConfigManager(), "ShowName");
    const bool showDistance = CONFIG_BOOL(GetConfigManager(), "ShowDistance");
    const bool showRole = CONFIG_BOOL(GetConfigManager(), "ShowRole");
    const bool showSnapline = CONFIG_BOOL(GetConfigManager(), "ShowSnapline");
    const bool showDead = CONFIG_BOOL(GetConfigManager(), "ShowDead");
    const bool showOnlyHunters = CONFIG_BOOL(GetConfigManager(), "ShowOnlyHunters");
    const bool showOnlySurvivors = CONFIG_BOOL(GetConfigManager(), "ShowOnlySurvivors");
    const bool boxEnabled = CONFIG_BOOL(GetConfigManager(), "BoxEnabled");
    const int boxType = CONFIG_INT(GetConfigManager(), "BoxType");
    const float boxThickness = CONFIG_FLOAT(GetConfigManager(), "BoxThickness");
    const float textScale = CONFIG_FLOAT(GetConfigManager(), "TextScale");
    const bool showLocalPlayer = CONFIG_BOOL(GetConfigManager(), "ShowLocalPlayer");
    const bool showSkeleton = CONFIG_BOOL(GetConfigManager(), "ShowSkeleton");
    const float skeletonThickness = CONFIG_FLOAT(GetConfigManager(), "SkeletonThickness");

    auto acknowledgedPawn = Utils::GetAcknowledgedPawn();
    auto drawList = ImGui::GetBackgroundDrawList();
    if (!drawList) return;

    ImFont* currentFont = ImGui::GetFont();
    float baseFontSize = ImGui::GetFontSize();

    int32_t numElements = playerArray.Num();
    if (numElements <= 0 || numElements > 200) return;

    for (int32_t i = 0; i < numElements; ++i)
    {
        std::remove_reference_t<decltype(playerArray[0])> player = nullptr;

        try
        {
            if (!playerArray.IsValidIndex(i))
                break;

            player = playerArray[i];
        }
        catch (const std::exception&)
        {
            break;
        }

        if (!player || !Utils::isObjectValid(player))
            continue;

        auto pawn = player->PawnPrivate;
        if (!pawn || !Utils::isObjectValid(pawn))
            continue;

        if (!pawn->IsA(SDK::ABP_FirstPersonCharacter_cLeon_Character_C::StaticClass()))
            continue;

        auto character = static_cast<SDK::ABP_FirstPersonCharacter_cLeon_Character_C*>(pawn);
        if (!character || !Utils::isObjectValid(character))
            continue;

        auto mesh = character->Mesh;
        if (!mesh || !Utils::isObjectValid(mesh))
            continue;

        if (!showLocalPlayer && (pawn == acknowledgedPawn || pawn == localPawn))
            continue;

        bool dead = mesh->IsAnySimulatingPhysics();
        if (!showDead && dead)
            continue;

        bool isHunter = pawn->IsA(SDK::ABP_FirstPersonCharacter_cLeon_Character_Hunter_C::StaticClass());
        bool isSurvivor = pawn->IsA(SDK::ABP_FirstPersonCharacter_cLeon_Character_Survivor_C::StaticClass());

        if (showOnlyHunters && !isHunter) continue;
        if (showOnlySurvivors && !isSurvivor) continue;

        ImU32 playerColor = GetPlayerColor(pawn, isHunter, false);

        if (showSkeleton)
            DrawSkeleton(mesh, localPC, drawList, playerColor, skeletonThickness);

        float minX, minY, maxX, maxY;
        if (!DrawBox(pawn, minX, minY, maxX, maxY))
            continue;

        if (boxEnabled)
        {
            switch (boxType)
            {
            case 0: DrawCornerBox(drawList, minX, minY, maxX, maxY, playerColor, boxThickness); break;
            case 1: DrawFullBox(drawList, minX, minY, maxX, maxY, playerColor, boxThickness); break;
            case 2: DrawRoundedBox(drawList, minX, minY, maxX, maxY, playerColor, boxThickness); break;
            }
        }

        if (showSnapline)
        {
            SDK::FVector pawnLoc = pawn->K2_GetActorLocation();
            SDK::FVector2D screenPos;
            if (Utils::WorldToScreen(pawnLoc, screenPos, false))
                DrawSnapline(drawList, screenPos, playerColor, 1.0f);
        }

        std::string renderText;
        float textYOffset = 0;

        if (showName)
        {
            if (player->PlayerNamePrivate.IsValid())
            {
                std::string nameStr = player->PlayerNamePrivate.ToString();
                if (!nameStr.empty())
                {
                    renderText += nameStr;
                    textYOffset += 1.0f;
                }
            }
            else
            {
                renderText += "Unknown";
                textYOffset += 1.0f;
            }
        }

        if (showRole)
        {
            std::string roleTxt = GetRoleText(pawn);
            if (!roleTxt.empty())
            {
                if (!renderText.empty()) renderText += " ";
                renderText += "[" + roleTxt + "]";
                textYOffset += 1.0f;
            }
        }

        if (showDistance && hasLocalLocation)
        {
            SDK::FVector enemyLoc = pawn->K2_GetActorLocation();
            SDK::FVector diff = enemyLoc - localLocation;
            float distance = sqrtf(diff.X * diff.X + diff.Y * diff.Y + diff.Z * diff.Z) / 100.0f;

            if (!renderText.empty()) renderText += " ";
            char distText[32];
            snprintf(distText, sizeof(distText), "[%dm]", static_cast<int>(distance));
            renderText += distText;
            textYOffset += 1.0f;
        }

        if (!renderText.empty())
        {
            ImVec2 textSize = ImGui::CalcTextSize(renderText.c_str());
            float scaledWidth = textSize.x * textScale;
            float scaledHeight = textSize.y * textScale;

            float textX = minX + (maxX - minX) * 0.5f - scaledWidth * 0.5f;
            float textY = minY - scaledHeight * textYOffset - 6.0f;

            ImU32 outlineColor = IM_COL32(0, 0, 0, 200);
            float scaledFontSize = baseFontSize * textScale;

            drawList->AddText(currentFont, scaledFontSize, ImVec2(textX - 1, textY), outlineColor, renderText.c_str());
            drawList->AddText(currentFont, scaledFontSize, ImVec2(textX + 1, textY), outlineColor, renderText.c_str());
            drawList->AddText(currentFont, scaledFontSize, ImVec2(textX, textY - 1), outlineColor, renderText.c_str());
            drawList->AddText(currentFont, scaledFontSize, ImVec2(textX, textY + 1), outlineColor, renderText.c_str());
            drawList->AddText(currentFont, scaledFontSize, ImVec2(textX, textY), playerColor, renderText.c_str());
        }
    }
}

void PlayerESP::OnMenuRender()
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

    bool enabled = IsActive();
    if (ImGui::Checkbox("Enable Player ESP", &enabled))
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

    ImGui::Separator();

    bool boxEnabled = CONFIG_BOOL(GetConfigManager(), "BoxEnabled");
    if (ImGui::Checkbox("Draw Box", &boxEnabled))
        SET_CONFIG_VALUE(GetConfigManager(), "BoxEnabled", bool, boxEnabled);

    if (boxEnabled)
    {
        int boxType = CONFIG_INT(GetConfigManager(), "BoxType");
        const char* boxTypes[] = { "Corner", "Full", "Rounded" };
        if (ImGui::Combo("Box Type", &boxType, boxTypes, IM_ARRAYSIZE(boxTypes)))
            SET_CONFIG_VALUE(GetConfigManager(), "BoxType", int, boxType);

        float thickness = CONFIG_FLOAT(GetConfigManager(), "BoxThickness");
        if (ImGui::SliderFloat("Box Thickness", &thickness, 1.0f, 5.0f))
            SET_CONFIG_VALUE(GetConfigManager(), "BoxThickness", float, thickness);
    }

    ImGui::Separator();

    bool showSkeleton = CONFIG_BOOL(GetConfigManager(), "ShowSkeleton");
    if (ImGui::Checkbox("Draw Skeleton", &showSkeleton))
        SET_CONFIG_VALUE(GetConfigManager(), "ShowSkeleton", bool, showSkeleton);

    if (showSkeleton)
    {
        float skThickness = CONFIG_FLOAT(GetConfigManager(), "SkeletonThickness");
        if (ImGui::SliderFloat("Skeleton Thickness", &skThickness, 0.5f, 4.0f))
            SET_CONFIG_VALUE(GetConfigManager(), "SkeletonThickness", float, skThickness);

        ImColor skColor = CONFIG_COLOR(GetConfigManager(), "SkeletonColor");
        if (ImGui::ColorEdit4("Skeleton Color", (float*)&skColor))
            SET_CONFIG_VALUE(GetConfigManager(), "SkeletonColor", ImColor, skColor);
    }

    ImGui::Separator();

    bool showName = CONFIG_BOOL(GetConfigManager(), "ShowName");
    if (ImGui::Checkbox("Show Name", &showName))
        SET_CONFIG_VALUE(GetConfigManager(), "ShowName", bool, showName);
    ImGui::SameLine();
    ImColor nameColor = CONFIG_COLOR(GetConfigManager(), "NameColor");
    if (ImGui::ColorEdit4("##NameColor", (float*)&nameColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
        SET_CONFIG_VALUE(GetConfigManager(), "NameColor", ImColor, nameColor);

    bool showRole = CONFIG_BOOL(GetConfigManager(), "ShowRole");
    if (ImGui::Checkbox("Show Role", &showRole))
        SET_CONFIG_VALUE(GetConfigManager(), "ShowRole", bool, showRole);

    bool showDistance = CONFIG_BOOL(GetConfigManager(), "ShowDistance");
    if (ImGui::Checkbox("Show Distance", &showDistance))
        SET_CONFIG_VALUE(GetConfigManager(), "ShowDistance", bool, showDistance);

    bool showSnapline = CONFIG_BOOL(GetConfigManager(), "ShowSnapline");
    if (ImGui::Checkbox("Show Snapline", &showSnapline))
        SET_CONFIG_VALUE(GetConfigManager(), "ShowSnapline", bool, showSnapline);
    ImGui::SameLine();
    ImColor snaplineColor = CONFIG_COLOR(GetConfigManager(), "SnaplineColor");
    if (ImGui::ColorEdit4("##SnaplineColor", (float*)&snaplineColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
        SET_CONFIG_VALUE(GetConfigManager(), "SnaplineColor", ImColor, snaplineColor);

    ImGui::Separator();

    bool showDead = CONFIG_BOOL(GetConfigManager(), "ShowDead");
    if (ImGui::Checkbox("Show Dead Players", &showDead))
        SET_CONFIG_VALUE(GetConfigManager(), "ShowDead", bool, showDead);

    bool showOnlyHunters = CONFIG_BOOL(GetConfigManager(), "ShowOnlyHunters");
    if (ImGui::Checkbox("Show Only Hunters", &showOnlyHunters))
        SET_CONFIG_VALUE(GetConfigManager(), "ShowOnlyHunters", bool, showOnlyHunters);

    bool showOnlySurvivors = CONFIG_BOOL(GetConfigManager(), "ShowOnlySurvivors");
    if (ImGui::Checkbox("Show Only Survivors", &showOnlySurvivors))
        SET_CONFIG_VALUE(GetConfigManager(), "ShowOnlySurvivors", bool, showOnlySurvivors);

    bool showLocalPlayer = CONFIG_BOOL(GetConfigManager(), "ShowLocalPlayer");
    if (ImGui::Checkbox("Show Local Player", &showLocalPlayer))
        SET_CONFIG_VALUE(GetConfigManager(), "ShowLocalPlayer", bool, showLocalPlayer);

    ImGui::Separator();

    float textScale = CONFIG_FLOAT(GetConfigManager(), "TextScale");
    if (ImGui::SliderFloat("Text Scale", &textScale, 0.5f, 2.0f))
        SET_CONFIG_VALUE(GetConfigManager(), "TextScale", float, textScale);

    ImGui::Separator();

    if (ImGui::CollapsingHeader("Colors"))
    {
        ImColor visibleCol = CONFIG_COLOR(GetConfigManager(), "VisibleColor");
        if (ImGui::ColorEdit4("Visible Color", (float*)&visibleCol))
            SET_CONFIG_VALUE(GetConfigManager(), "VisibleColor", ImColor, visibleCol);

        ImColor invisibleCol = CONFIG_COLOR(GetConfigManager(), "InvisibleColor");
        if (ImGui::ColorEdit4("Invisible Color", (float*)&invisibleCol))
            SET_CONFIG_VALUE(GetConfigManager(), "InvisibleColor", ImColor, invisibleCol);

        ImColor hunterCol = CONFIG_COLOR(GetConfigManager(), "HunterColor");
        if (ImGui::ColorEdit4("Hunter Color", (float*)&hunterCol))
            SET_CONFIG_VALUE(GetConfigManager(), "HunterColor", ImColor, hunterCol);

        ImColor survivorCol = CONFIG_COLOR(GetConfigManager(), "SurvivorColor");
        if (ImGui::ColorEdit4("Survivor Color", (float*)&survivorCol))
            SET_CONFIG_VALUE(GetConfigManager(), "SurvivorColor", ImColor, survivorCol);
    }

    ImGui::PopStyleVar();
}