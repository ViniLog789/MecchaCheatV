#include "watermark.h"

using namespace MecchaCheatV::Features::Visuals;

Watermark::Watermark() : FeatureCore("Watermark", TYPE_VISUALS) 
{
	DECLARE_CONFIG(GetConfigManager(), "ShowFPS", bool, false);

	CachedText.reserve(128);
}

void Watermark::OnRender()
{
    if (!IsActive()) return;

    const bool showFPS = CONFIG_BOOL(GetConfigManager(), "ShowFPS");

    CachedText = "MecchaCheatV";

    if (showFPS)
    {
        const int fps = static_cast<int>(ImGui::GetIO().Framerate);
        CachedText.append(" | ");
        CachedText.append(std::to_string(fps));
        CachedText.append(" FPS");
    }

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[3]);
    const ImVec2 textSize = ImGui::CalcTextSize(CachedText.c_str());
    ImGui::PopFont();

    const float width = textSize.x + 24.0f;
    const float height = textSize.y + 10.0f;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const float padding = 10.0f;
    ImVec2 defaultPos = ImVec2(
        viewport->Size.x - padding - width,
        padding
    );
    ImGui::SetNextWindowPos(defaultPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(width, height));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    ImGui::Begin("###WatermarkWindow", nullptr, Globals::WINDOW_FLAGS_GLOBALS);
    {
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();

        ImDrawList* drawList = ImGui::GetWindowDrawList();

        drawList->AddRectFilled(
            windowPos,
            ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y),
            IM_COL32(15, 15, 15, 180),
            12.0f
        );

        drawList->AddRect(
            windowPos,
            ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y),
            IM_COL32(80, 50, 120, 200),
            12.0f,
            0,
            1.5f
        );

        const ImVec2 textPos = ImVec2(
            windowPos.x + (windowSize.x - textSize.x) / 2,
            windowPos.y + (windowSize.y - textSize.y) / 2
        );

        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[3]);
        drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), CachedText.c_str());
        ImGui::PopFont();
    }
    ImGui::End();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar(4);
}


void Watermark::OnMenuRender()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

	bool enabled = IsActive();
	if (ImGui::Checkbox("Enable watermark", &enabled))
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

	bool showFPS = CONFIG_BOOL(GetConfigManager(), "ShowFPS");
	if (ImGui::Checkbox("Show FPS", &showFPS))
        SET_CONFIG_VALUE(GetConfigManager(), "ShowFPS", bool, showFPS);

	ImGui::PopStyleVar();
}