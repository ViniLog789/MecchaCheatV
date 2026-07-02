// Some code was taken from phxgg!
#include "decoyesp.h"
#include <algorithm>

using namespace MecchaCheatV::Features::Visuals;

DecoyESP::DecoyESP() : FeatureCore("Decoy ESP", TYPE_VISUALS)
{
    DECLARE_CONFIG(GetConfigManager(), "BoxEnabled", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "BoxType", int, 0);
    DECLARE_CONFIG(GetConfigManager(), "BoxThickness", float, 2.0f);
    DECLARE_CONFIG(GetConfigManager(), "ShowName", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "ShowSnapline", bool, false);
    DECLARE_CONFIG(GetConfigManager(), "TextScale", float, 1.0f);
    DECLARE_CONFIG(GetConfigManager(), "DecoyColor", ImColor, ImColor(255, 105, 180, 255));
}

bool DecoyESP::DrawBox(SDK::AActor* actor, float& outMinX, float& outMinY, float& outMaxX, float& outMaxY)
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

void DecoyESP::DrawCornerBox(ImDrawList* drawList, float minX, float minY, float maxX, float maxY, ImU32 color, float thickness)
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

void DecoyESP::DrawFullBox(ImDrawList* drawList, float minX, float minY, float maxX, float maxY, ImU32 color, float thickness)
{
    ImU32 outlineColor = IM_COL32(0, 0, 0, 180);
    float outlineThickness = thickness + 2.0f;

    drawList->AddRect(ImVec2(minX, minY), ImVec2(maxX, maxY), outlineColor, 0.0f, 0, outlineThickness);
    drawList->AddRect(ImVec2(minX, minY), ImVec2(maxX, maxY), color, 0.0f, 0, thickness);
}

void DecoyESP::DrawRoundedBox(ImDrawList* drawList, float minX, float minY, float maxX, float maxY, ImU32 color, float thickness)
{
    float radius = (maxX - minX) * 0.08f;
    radius = std::clamp(radius, 2.0f, 8.0f);

    ImU32 outlineColor = IM_COL32(0, 0, 0, 180);
    float outlineThickness = thickness + 2.0f;

    drawList->AddRect(ImVec2(minX, minY), ImVec2(maxX, maxY), outlineColor, radius, 0, outlineThickness);
    drawList->AddRect(ImVec2(minX, minY), ImVec2(maxX, maxY), color, radius, 0, thickness);
}

void DecoyESP::DrawSnapline(ImDrawList* drawList, const SDK::FVector2D& target, ImU32 color, float thickness)
{
    ImVec2 screenCenter(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y);
    drawList->AddLine(screenCenter, ImVec2(target.X, target.Y), color, thickness);
}

ImU32 DecoyESP::GetDecoyColor()
{
    return CONFIG_COLOR(GetConfigManager(), "DecoyColor");
}

void DecoyESP::OnRender()
{
    if (!IsActive()) return;

    auto world = SDK::UWorld::GetWorld();
    if (!world || !Utils::isObjectValid(world) || !Utils::IsInMatch(world)) return;

    const bool showBox = CONFIG_BOOL(GetConfigManager(), "BoxEnabled");
    const int boxType = CONFIG_INT(GetConfigManager(), "BoxType");
    const float boxThickness = CONFIG_FLOAT(GetConfigManager(), "BoxThickness");
    const bool showName = CONFIG_BOOL(GetConfigManager(), "ShowName");
    const bool showSnapline = CONFIG_BOOL(GetConfigManager(), "ShowSnapline");
    const float textScale = CONFIG_FLOAT(GetConfigManager(), "TextScale");

    auto drawList = ImGui::GetBackgroundDrawList();
    if (!drawList) return;

    ImFont* currentFont = ImGui::GetFont();
    float baseFontSize = ImGui::GetFontSize();

    SDK::UClass* decoyClass = SDK::ABP_cLeonDecoy_Base_C::StaticClass();
    if (!decoyClass || !Utils::isObjectValid(decoyClass)) return;

    SDK::TArray<SDK::AActor*> Decoys;

    try
    {
        SDK::UGameplayStatics::GetAllActorsOfClass(world, decoyClass, &Decoys);
    }
    catch (...)
    {
        return;
    }

    if (!Decoys.IsValid()) return;

    int32_t numElements = Decoys.Num();
    if (numElements <= 0 || numElements > 500) return;

    for (int i = 0; i < numElements; ++i)
    {
        if (!Decoys.IsValidIndex(i)) break;

        auto* actor = Decoys[i];
        if (!actor || !Utils::isObjectValid(actor)) continue;

        auto* decoy = static_cast<SDK::ABP_cLeonDecoy_Base_C*>(actor);
        if (!decoy || !Utils::isObjectValid(decoy)) continue;

        auto* mesh = decoy->PoseableMesh;
        if (!mesh || !Utils::isObjectValid(mesh)) continue;

        if (!mesh->bVisible) continue;

        float minX, minY, maxX, maxY;
        if (!DrawBox(decoy, minX, minY, maxX, maxY))
            continue;

        ImU32 decoyColor = GetDecoyColor();

        if (showBox)
        {
            switch (boxType)
            {
            case 0: DrawCornerBox(drawList, minX, minY, maxX, maxY, decoyColor, boxThickness); break;
            case 1: DrawFullBox(drawList, minX, minY, maxX, maxY, decoyColor, boxThickness); break;
            case 2: DrawRoundedBox(drawList, minX, minY, maxX, maxY, decoyColor, boxThickness); break;
            }
        }

        if (showSnapline)
        {
            SDK::FVector decoyLoc = decoy->K2_GetActorLocation();
            SDK::FVector2D screenPos;
            if (Utils::WorldToScreen(decoyLoc, screenPos, false))
                DrawSnapline(drawList, screenPos, decoyColor, 1.0f);
        }

        if (showName)
        {
            const char* renderText = "Decoy";

            ImVec2 textSize = ImGui::CalcTextSize(renderText);
            float scaledWidth = textSize.x * textScale;
            float scaledHeight = textSize.y * textScale;

            float textX = minX + (maxX - minX) * 0.5f - scaledWidth * 0.5f;
            float textY = minY - scaledHeight - 6.0f;

            ImU32 outlineColor = IM_COL32(0, 0, 0, 200);
            float scaledFontSize = baseFontSize * textScale;

            drawList->AddText(currentFont, scaledFontSize, ImVec2(textX - 1, textY), outlineColor, renderText);
            drawList->AddText(currentFont, scaledFontSize, ImVec2(textX + 1, textY), outlineColor, renderText);
            drawList->AddText(currentFont, scaledFontSize, ImVec2(textX, textY - 1), outlineColor, renderText);
            drawList->AddText(currentFont, scaledFontSize, ImVec2(textX, textY + 1), outlineColor, renderText);
            drawList->AddText(currentFont, scaledFontSize, ImVec2(textX, textY), decoyColor, renderText);
        }
    }
}

void DecoyESP::OnMenuRender()
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

    bool enabled = IsActive();
    if (ImGui::Checkbox("Enable Decoy ESP", &enabled))
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

    bool showBox = CONFIG_BOOL(GetConfigManager(), "BoxEnabled");
    if (ImGui::Checkbox("Draw Box", &showBox))
        SET_CONFIG_VALUE(GetConfigManager(), "BoxEnabled", bool, showBox);

    if (showBox)
    {
        int boxType = CONFIG_INT(GetConfigManager(), "BoxType");
        const char* boxTypes[] = { "Corner", "Full", "Rounded" };
        if (ImGui::Combo("Box Type", &boxType, boxTypes, IM_ARRAYSIZE(boxTypes)))
            SET_CONFIG_VALUE(GetConfigManager(), "BoxType", int, boxType);

        float thickness = CONFIG_FLOAT(GetConfigManager(), "BoxThickness");
        if (ImGui::SliderFloat("Box Thickness", &thickness, 1.0f, 5.0f))
            SET_CONFIG_VALUE(GetConfigManager(), "BoxThickness", float, thickness);
    }

    bool showName = CONFIG_BOOL(GetConfigManager(), "ShowName");
    if (ImGui::Checkbox("Show Name", &showName))
        SET_CONFIG_VALUE(GetConfigManager(), "ShowName", bool, showName);

    bool showSnapline = CONFIG_BOOL(GetConfigManager(), "ShowSnapline");
    if (ImGui::Checkbox("Show Snapline", &showSnapline))
        SET_CONFIG_VALUE(GetConfigManager(), "ShowSnapline", bool, showSnapline);

    float textScale = CONFIG_FLOAT(GetConfigManager(), "TextScale");
    if (ImGui::SliderFloat("Text Scale", &textScale, 0.5f, 2.0f))
        SET_CONFIG_VALUE(GetConfigManager(), "TextScale", float, textScale);

    ImGui::Separator();

    ImColor decoyCol = CONFIG_COLOR(GetConfigManager(), "DecoyColor");
    if (ImGui::ColorEdit4("Decoy Color", (float*)&decoyCol))
        SET_CONFIG_VALUE(GetConfigManager(), "DecoyColor", ImColor, decoyCol);

    ImGui::PopStyleVar();
}