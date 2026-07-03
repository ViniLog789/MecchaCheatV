#include "menu.h"
#include "styles.h"
#include "../features/features_includes.h"
inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }

using namespace MecchaCheatV;

namespace fs = std::filesystem;

void Menu::Initialize() {
    SetMenuDefaultStyle();
    InitFonts();
    Initialized = true;
}

void Menu::NewYear()
{
    static std::vector<ImVec2> snow(80);
    static bool init = false;
    if (!init) {
        init = true;
        for (auto& s : snow)
            s = ImVec2((float)(rand() % (int)ImGui::GetIO().DisplaySize.x),
                (float)(rand() % (int)ImGui::GetIO().DisplaySize.y));
    }

    auto& io = ImGui::GetIO();
    ImDrawList* dl = ImGui::GetBackgroundDrawList();

    ImGui::PushFont(io.Fonts->Fonts[4]);
    ImU32 snowColor = IM_COL32(230, 230, 255, 200);
    for (auto& s : snow) {
        dl->AddText(s, snowColor, "S");
        s.y += 0.4f + (rand() % 100) / 200.f;
        if (s.y > io.DisplaySize.y)
            s = ImVec2((float)(rand() % (int)io.DisplaySize.x), -10.f);
    }
    ImGui::PopFont();
}


void Menu::Render()
{
    constexpr ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse;

    constexpr int menuWidth = 900;
    constexpr int menuHeight = 650;
    constexpr float tabBarWidth = 200.f;

    ImGui::SetNextWindowSize(ImVec2(menuWidth * dpiScale, menuHeight * dpiScale), ImGuiCond_Once);
    ImGui::SetNextWindowBgAlpha(1.0f);

    ImGui::Begin("##MainMenu", nullptr, windowFlags);

    ImVec2 contentSize = ImGui::GetContentRegionAvail();
    ImGui::BeginChild("MainLayout", contentSize, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::BeginChild("TabBar", ImVec2(tabBarWidth * dpiScale, contentSize.y), true);
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 4));

        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
        ImGui::SetCursorPosY(20 * dpiScale);

        ImGui::SetCursorPosX((tabBarWidth * dpiScale - ImGui::CalcTextSize("MecchaCheatV").x) * 0.5f);
        ImGui::TextColored(accentPurple, "MecchaCheatV");

        ImGui::SetCursorPosX((tabBarWidth * dpiScale - ImGui::CalcTextSize("by VCom Team").x) * 0.5f);
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.8f, 0.8f), "by VCom Team");
        ImGui::PopFont();

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10 * dpiScale);
        ImGui::Separator();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 * dpiScale);

        auto DrawTabButton = [&](const char* label, int tabIndex) {
            bool isActive = menu.currentTab == tabIndex;
            ImVec2 sz(tabBarWidth * dpiScale - 40, 50 * dpiScale);
            float x = (tabBarWidth * dpiScale - sz.x) * 0.5f;
            ImGui::SetCursorPosX(x);

            ImGui::PushStyleColor(ImGuiCol_Button, isActive ? accentPurpleDark : ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, isActive ? accentPurple : ImVec4(0.15f, 0.15f, 0.15f, 1));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, accentPurpleLight);
            ImGui::PushStyleColor(ImGuiCol_Text, isActive ? ImVec4(1, 1, 1, 1) : ImVec4(0.8f, 0.8f, 0.9f, 1));

            if (ImGui::Button(label, sz)) menu.currentTab = tabIndex;

            if (isActive) {
                auto p0 = ImGui::GetItemRectMin();
                auto p1 = ImGui::GetItemRectMax();
                ImGui::GetWindowDrawList()->AddRectFilled(
                    ImVec2(p0.x, p1.y - 2), p1, ImGui::GetColorU32(accentPurpleLight)
                );
            }

            ImGui::PopStyleColor(4);
            };

        DrawTabButton("Features", 0);
        DrawTabButton("Settings", 1);
        DrawTabButton("About", 2);
        if (IsDebugging)
            DrawTabButton("TEST", 3);

        float bottomBlock = 120 * dpiScale;

        ImGui::SetCursorPosY(contentSize.y - bottomBlock);

        ImGui::Separator();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 * dpiScale);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 0.3f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.2f, 0.2f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.6f, 0.6f, 1.0f));

        ImVec2 sz(tabBarWidth * dpiScale - 40, 45 * dpiScale);
        float x = (tabBarWidth * dpiScale - sz.x) * 0.5f;
        ImGui::SetCursorPosX(x);

        if (ImGui::Button("Unload", sz)) {
            menu.Toggle();
            CheatWork = false;
        }

        ImGui::PopStyleColor(4);

        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 30 * dpiScale);
        ImGui::Separator();

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.5 * dpiScale);
        std::string ver = Version + " | MIT License";

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.5 * dpiScale);
        ImGui::SetCursorPosX((tabBarWidth * dpiScale - ImGui::CalcTextSize(ver.c_str()).x) * 0.5f);

        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.6f, 0.7f), "%s", ver.c_str());

        ImGui::PopStyleVar();
    }

    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("ContentArea", ImVec2(contentSize.x - tabBarWidth * dpiScale, contentSize.y), true);
    {
        switch (menu.currentTab)
        {
        case 0:
            ImGui::BeginChild("FeaturesContent", ImVec2(0, 0), true);
            {
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10 * dpiScale);

                if (GET_FEATURE_HANDLER())
                {
                    GET_FEATURE_HANDLER()->RenderMenu();
                }
                else
                {
                    ImGui::SetCursorPos(ImVec2(
                        (ImGui::GetWindowWidth() - ImGui::CalcTextSize("No features...").x) * 0.5f,
                        (ImGui::GetWindowHeight() - ImGui::GetTextLineHeight()) * 0.5f
                    ));
                    ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "No features...");
                }
            }
            ImGui::EndChild();
            break;

        case 1:
        {
            ImGui::SetCursorPos(ImVec2(25 * dpiScale, 20 * dpiScale));

            ImGui::BeginGroup();

            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
            ImGui::TextColored(accentPurple, "Settings");
            ImGui::PopFont();

            ImGui::Separator();
            ImGui::Dummy(ImVec2(0, 10));

            ImGui::BeginChild("GeneralSettings", ImVec2(0, 0), true);
            {
                ImGui::Columns(2, nullptr, false);
                {
                    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[5]);
                    ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.f, 1.f), "General");
                    ImGui::PopFont();

                    ImGui::Spacing();

                    ImGui::Text("Toggle key: ");

                    ImGui::PushStyleColor(ImGuiCol_Button, accentPurpleDark);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, accentPurple);

                    if (ImGui::Button("Set key", ImVec2(140 * dpiScale, 28 * dpiScale)))
                    {
                        ImGui::OpenPopup("Set menu key");
                    }

                    ImGui::PopStyleColor(2);

                    ImGui::SameLine();
                    ImGui::TextColored(accentPurpleLight, Utils::getKeyName(MenuToggleKey).c_str());

                    ImGui::Spacing();
                    if (ImGui::Button("Clear cache", ImVec2(160 * dpiScale, 30 * dpiScale)))
                    {
                        std::string logsPath = Utils::GetCheatDirectory() + "\\logs";

                        try
                        {
                            int deleted = 0;
                            for (const auto& entry : fs::directory_iterator(logsPath))
                            {
                                if (entry.is_regular_file() && fs::remove(entry.path()))
                                    deleted++;
                            }
                            LOG_INFO("Logs cleaned: ", deleted);
                        }
                        catch (...) {}
                    }
                }

                ImGui::NextColumn();
                {
                    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[5]);
                    ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.f, 1.f), "Colors");
                    ImGui::PopFont();

                    ImGui::Spacing();

                    bool changed = false;

                    changed |= ImGui::ColorEdit4("Accent", (float*)&Globals::accentPurple);
                    changed |= ImGui::ColorEdit4("Accent Dark", (float*)&Globals::accentPurpleDark);
                    changed |= ImGui::ColorEdit4("Accent Light", (float*)&Globals::accentPurpleLight);

                    changed |= ImGui::ColorEdit4("Dark BG", (float*)&Globals::darkBg);
                    changed |= ImGui::ColorEdit4("Darker BG", (float*)&Globals::darkerBg);
                    changed |= ImGui::ColorEdit4("Card BG", (float*)&Globals::cardBg);
                    changed |= ImGui::ColorEdit4("Header BG", (float*)&Globals::headerBg);

                    ImGui::Spacing();

                    if (ImGui::Button("Save Colors", ImVec2(160 * dpiScale, 30 * dpiScale)))
                    {
                        Config::SaveConfig();
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Reset Colors", ImVec2(140 * dpiScale, 30 * dpiScale)))
                    {
                        Globals::ResetColors();
                        Config::SaveConfig();
                    }

                    if (changed)
                    {
                        Config::SaveConfig();
                    }
                }

                ImGui::Columns(1);

                if (ImGui::BeginPopupModal("Set menu key", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::Text("Press key");
                    ImGui::Separator();

                    if (ImGui::Button("Cancel"))
                        ImGui::CloseCurrentPopup();

                    for (int key = 0; key < 256; key++)
                    {
                        if (GetAsyncKeyState(key) & 0x8000)
                        {
                            MenuToggleKey = key;
                            ImGui::CloseCurrentPopup();
                            break;
                        }
                    }

                    ImGui::EndPopup();
                }
            }
            ImGui::EndChild();

            ImGui::EndGroup();
        }
        break;
        case 2:
        {
            ImGui::SetCursorPosY(20 * dpiScale);
            ImGui::SetCursorPosX(25 * dpiScale);
            ImGui::BeginGroup();

            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
            ImGui::TextColored(accentPurple, "About");
            ImGui::PopFont();
            ImGui::Separator();
            ImGui::Dummy(ImVec2(0, 15));

            ImGui::BeginChild("OpenSource", ImVec2(0, 150 * dpiScale), true);
            {
                ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[5]);
                ImGui::TextColored(accentPurple, "Open Source");
                ImGui::PopFont();
                ImGui::Separator();
                ImGui::Dummy(ImVec2(0, 9));

                ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), "GitHub Repository");

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.4f, 1.0f));

                const char* text = "Open GitHub repository";
                ImVec2 textSize = ImGui::CalcTextSize(text);

                ImVec2 padding = ImGui::GetStyle().FramePadding;

                ImVec2 buttonSize = ImVec2(
                    textSize.x + padding.x * 2.0f,
                    textSize.y + padding.y * 2.0f
                );

                if (ImGui::Button(text, buttonSize))
                {
                    const char* repo = GitHubRepository;
                    ShellExecuteA(0, "Open", repo, 0, 0, SW_SHOW);
                }

                ImGui::PopStyleColor(2);

                ImGui::Dummy(ImVec2(0, 8));
                ImGui::TextWrapped("This project is open source and available under MIT License. Feel free to contribute!");
            }
            ImGui::EndChild();

            ImGui::BeginChild("Credits", ImVec2(0, 0), true);
            {
                ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[5]);
                ImGui::TextColored(accentPurple, "Credits");
                ImGui::PopFont();
                ImGui::Separator();
                ImGui::Dummy(ImVec2(0, 10));

                ImGui::Columns(2, "TeamColumns", false);
                ImGui::SetColumnWidth(0, 250 * dpiScale);

                ImGui::BulletText("ViniLog");

                ImGui::Columns(1);

                ImGui::Dummy(ImVec2(0, 8));
                ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), "Special Thanks");
                ImGui::TextWrapped("Evelien (feature system), .gashopeless, crymore.vip");
            }
            ImGui::EndChild();

            ImGui::EndGroup();
        }
        break;
        case 3:
            if (IsDebugging)
            {
                ImGui::BeginChild("TestContent", ImVec2(0, 0), true);

                if (ImGui::Button("Crash game"))
                {
                    *(int*)0 = 42;
                }

				if (ImGui::Button("Test IsServer"))
				{
					bool a1 = SDK::UKismetSystemLibrary::IsServer(SDK::UWorld::GetWorld());
                    LOG_INFO(a1 ? "yes" : "no");
				}

                if (ImGui::Button("Call test"))
                    Globals::ForTests = true;

                ImGui::EndChild();
            }
            break;
        }
    }
    ImGui::EndChild();
    ImGui::EndChild();
    ImGui::End();
}

void Menu::Toggle()
{
    menu.Open = !menu.Open;
}