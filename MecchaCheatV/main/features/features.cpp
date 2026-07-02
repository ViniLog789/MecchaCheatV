#include "features.h"
#include "features_includes.h"

using namespace MecchaCheatV;
using namespace Features::Visuals;
using namespace Features::Player;

/* 
=== ICONS ===
    Visuals - V
    Movement - L
    Misc - M
    Player - P
*/

const char* MecchaCheatV::GetFeatureTypeIcon(const FeatureType type)
{
    switch (type)
    {
    case TYPE_NONE: return "N";
    case TYPE_VISUALS: return "V";
    case TYPE_PLAYER: return "P";
    default: return "U";
    }
}

const char* MecchaCheatV::GetFeatureTypeName(const FeatureType type)
{
    switch (type)
    {
    case TYPE_NONE: return "None";
    case TYPE_VISUALS: return "Visuals";
    case TYPE_PLAYER: return "Player(s)";
    default: return "Unknown";
    }
}

FeatureHandler::FeatureHandler() : CurrentType(TYPE_NONE)
{
    FeatureRegistry = std::vector<std::pair<std::string, std::unique_ptr<FeatureCore>>>();

    // Visuals
    ADD_FEATURE(this, Watermark);
    ADD_FEATURE(this, PlayerESP);
    ADD_FEATURE(this, DecoyESP);

    // Player(s)
	ADD_FEATURE(this, FOVChanger);
	ADD_FEATURE(this, SetName);
	ADD_FEATURE(this, AlwaysVisible);
	ADD_FEATURE(this, AutoDissShadow);
	ADD_FEATURE(this, Teleport);

    MainFeatureHandler = this;
}

FeatureHandler::~FeatureHandler()
{
    for (auto& [name, feature] : FeatureRegistry)
    {
        if (feature && feature->IsActive())
        {
            feature->OnDeactivate();
        }
    }

    MainFeatureHandler = nullptr;
}

void FeatureHandler::RegisterFeature(std::string_view name, std::unique_ptr<FeatureCore> feature)
{
    auto* ptr = feature.get();
    FeatureRegistry.emplace_back(std::string(name), std::move(feature));
    if (ptr->IsActive())
        ptr->OnActivate();
}

FeatureCore* FeatureHandler::FindFeature(const std::string_view name) const
{
    for (const auto& [featureName, feature] : FeatureRegistry)
    {
        if (featureName == name)
            return feature.get();
    }
    return nullptr;
}

void FeatureHandler::RenderAll() const
{
    for (const auto& [name, feature] : FeatureRegistry)
    {
        if (feature->IsActive())
        {
            feature->OnRender();
        }
    }
}

bool FeatureHandler::FeatureMatchesSearch(const std::string& featureName, const std::string& query) const
{
    if (query.empty()) return false;

    auto it = std::search(
        featureName.begin(), featureName.end(),
        query.begin(), query.end(),
        [](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
    );

    return (it != featureName.end());
}

void FeatureHandler::ShowTypeSelector()
{
    constexpr std::array<FeatureType, 2> TYPES = { TYPE_VISUALS, TYPE_PLAYER };

    const float width = ImGui::GetContentRegionAvail().x;
    const float btnSize = (width - 20.0f * 2) / 3.0f;
    const ImVec2 size(btnSize, btnSize);

    ImGui::SetNextItemWidth(width - 20.f);
    ImGui::InputTextWithHint("##SearchFeatures", "Search features...", SearchBuffer, sizeof(SearchBuffer));
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (SearchBuffer[0] != '\0')
    {
        std::string query = SearchBuffer;
        bool foundAny = false;

        ImGui::BeginChild("SearchResults", ImVec2(0, 0), true);

        for (const auto& [name, feature] : FeatureRegistry)
        {
            if (FeatureMatchesSearch(name, query) || FeatureMatchesSearch(feature->GetFeatureTitle(), query))
            {
                if (feature->ShowInMenu)
                {
                    ImGui::PushID(name.c_str());
                    ImGui::TextDisabled("[%s]", GetFeatureTypeName(feature->GetFeatureType()));
                    ImGui::SameLine();
                    ImGui::SeparatorText(feature->GetFeatureTitle().c_str());
                    feature->OnMenuRender();
                    ImGui::Spacing();
                    ImGui::PopID();
                    foundAny = true;
                }
            }
        }

        if (!foundAny)
            ImGui::TextDisabled("No features found");

        ImGui::EndChild();
        return;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 12));
    ImGui::Spacing();
    ImGui::BeginGroup();

    int count = 0;
    for (FeatureType type : TYPES)
    {
        if (count > 0 && count % 3 != 0)
            ImGui::SameLine();

        const char* icon = GetFeatureTypeIcon(type);
        const char* label = GetFeatureTypeName(type);

        if (ImGui::Button(("##btn_" + std::to_string(count)).c_str(), size))
            CurrentType = type;

        const ImVec2 pos = ImGui::GetItemRectMin();
        const ImVec2 itemSize = ImGui::GetItemRectSize();
        const ImVec2 center(pos.x + itemSize.x * 0.5f, pos.y + itemSize.y * 0.5f);
        auto* draw = ImGui::GetWindowDrawList();

        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
        ImVec2 iconText = ImGui::CalcTextSize(icon);
        draw->AddText(ImVec2(center.x - iconText.x * 0.5f, center.y - iconText.y * 0.5f - 6.0f),
            IM_COL32_WHITE, icon);
        ImGui::PopFont();

        ImVec2 textSize = ImGui::CalcTextSize(label);
        draw->AddText(ImVec2(center.x - textSize.x * 0.5f, pos.y + itemSize.y - textSize.y - 6.0f),
            IM_COL32_WHITE, label);

        count++;
    }

    ImGui::EndGroup();
    ImGui::PopStyleVar(2);
}

void FeatureHandler::ShowFeaturesByType()
{
    const char* categoryName = GetFeatureTypeName(CurrentType);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[5]);
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%s Features", categoryName);
    ImGui::PopFont();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Back", ImVec2(150, 30)))
    {
        CurrentType = TYPE_NONE;
        return;
    }

    ImGui::Spacing();
    ImGui::BeginChild("Features", ImVec2(0, 0), true);

    for (const auto& [name, feature] : FeatureRegistry)
    {
        if (feature->GetFeatureType() == CurrentType && feature->ShowInMenu)
        {
            ImGui::PushID(name.c_str());
            ImGui::SeparatorText(feature->GetFeatureTitle().c_str());
            feature->OnMenuRender();
            ImGui::Spacing();
            ImGui::PopID();
        }
    }

    ImGui::EndChild();
}

void FeatureHandler::ApplyConfigStates()
{
    for (auto& [name, feature] : FeatureRegistry)
    {
        if (feature->IsActive())
            feature->OnActivate();
    }
}

void FeatureHandler::RenderMenu()
{
    if (CurrentType == TYPE_NONE)
    {
        ShowTypeSelector();
    }
    else
    {
        ShowFeaturesByType();
    }
}