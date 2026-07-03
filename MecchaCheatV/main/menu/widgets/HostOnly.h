#include "Includes.h"

namespace ImGui
{
    inline bool IsHost()
    {
        return SDK::UWorld::GetWorld() && SDK::UKismetSystemLibrary::IsServer(SDK::UWorld::GetWorld());
    }

    inline void BeginHostOnly()
    {
        ImGui::BeginGroup();

        if (!IsHost())
        {
            ImGui::BeginDisabled();
        }
    }

    inline void EndHostOnly()
    {
        bool host = IsHost();

        if (!host)
        {
            ImGui::EndDisabled();
        }

        ImGui::EndGroup();

        if (!host)
        {
            ImVec2 min = ImGui::GetItemRectMin();
            ImVec2 max = ImGui::GetItemRectMax();

            min.x -= 4.0f; min.y -= 4.0f;
            max.x += 4.0f; max.y += 4.0f;

            ImDrawList* drawList = ImGui::GetWindowDrawList();

            drawList->AddRectFilled(min, max, IM_COL32(15, 15, 15, 210), 4.0f);
            drawList->AddRect(min, max, IM_COL32(255, 60, 60, 150), 4.0f, 0, 1.0f);

            const char* text1 = "Locked. You are ";
            const char* text2 = "NOT host";

            ImVec2 size1 = ImGui::CalcTextSize(text1);
            ImVec2 size2 = ImGui::CalcTextSize(text2);

            float totalWidth = size1.x + size2.x;
            float textHeight = size1.y;

            ImVec2 center = ImGui::GetItemRectMin();
            ImVec2 maxRect = ImGui::GetItemRectMax();
            center = ImVec2((center.x + maxRect.x) * 0.5f, (center.y + maxRect.y) * 0.5f);

            ImVec2 startPos = ImVec2(center.x - totalWidth * 0.5f, center.y - textHeight * 0.5f);

            drawList->AddText(startPos, IM_COL32(200, 200, 200, 255), text1);

            ImVec2 pos2 = ImVec2(startPos.x + size1.x, startPos.y);
            drawList->AddText(pos2, IM_COL32(255, 80, 80, 255), text2);

            ImVec2 lineStart = ImVec2(pos2.x, pos2.y + textHeight + 1.0f);
            ImVec2 lineEnd = ImVec2(pos2.x + size2.x, pos2.y + textHeight + 1.0f);

            drawList->AddLine(lineStart, lineEnd, IM_COL32(255, 80, 80, 255), 1.5f);
        }
    }
}