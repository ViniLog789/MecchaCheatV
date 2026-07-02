#include "Includes.h"
#include <TlHelp32.h>
#include "memory.h"

using namespace MecchaCheatV;

static std::unique_ptr<Logger> loggerInstance;
static std::unique_ptr<Renderer> rendererInstance;
static std::unique_ptr<Hooking> hookingInstance;
static std::unique_ptr<FeatureHandler> featureInstance;

// Main cheat thread
extern "C" __declspec(dllexport) DWORD WINAPI MecchaCheatVThread()
{
    Utils::CreateCheatDirectory();

    bool hooksApplied = false;

    try {
        loggerInstance = std::make_unique<Logger>(Logger::Level::Call);
    }
    catch (...) {
        return 0;
    }

    try {
        rendererInstance = std::make_unique<Renderer>();
        hookingInstance = std::make_unique<Hooking>();
        featureInstance = std::make_unique<FeatureHandler>();

        hookingInstance->OriginalPresent = rendererInstance->GetPresent();

        Config::LoadConfig();

        if (IsDebugging)
            LOG_WARN("The build is built with the IsDebugging flag enabled.");

        Globals::hookedProcessEvent = reinterpret_cast<ProcessEvent_t>(SDK::InSDKUtils::GetImageBase() + SDK::Offsets::ProcessEvent);

        AHK(hookingInstance->OriginalPresent, Hooks::HkPresent); // ADD_HOOK
        AHK(Globals::hookedProcessEvent, Hooks::HkProcessEvent);

        hookingInstance->ApplyHooks();
        hooksApplied = true;

        NOTIFY_INFO_QUICK("Cheat injected successfully. The menu opens on " + Utils::getKeyName(MenuToggleKey));
        LOG_INFO("Cheat injected successfully. The menu opens on " + Utils::getKeyName(MenuToggleKey));

        while (Globals::CheatWork)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    catch (const std::exception& e) {
        LOG_ERROR(std::string("Exception in main thread: ") + e.what());
    }

    LOG_INFO("Starting cleanup...");

    try {
        Config::SaveConfig();

        if (rendererInstance) {
            ImGui::SaveIniSettingsToDisk((Utils::GetCheatDirectory() + "\\menu.ini").c_str());
        }
    }
    catch (...) {}

    if (hooksApplied && hookingInstance)
    {
        try {
            hookingInstance->RemoveHooks();
        }
        catch (...) {}
        hooksApplied = false;
    }

    hookingInstance.reset();
    rendererInstance.reset();
    featureInstance.reset();

    LOG_INFO("Cleanup completed");

//finalize:
    loggerInstance.reset();

    FreeLibraryAndExitThread(Globals::globalModule, NULL);
    return 0;
}

// DLL entry point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
    {
        Globals::CheatWork = true;
        DisableThreadLibraryCalls(hModule);
        Globals::globalModule = hModule;

        HANDLE hThread = CreateThread(nullptr, 0,
            reinterpret_cast<LPTHREAD_START_ROUTINE>(MecchaCheatVThread),
            nullptr, 0, nullptr);

        if (hThread)
            CloseHandle(hThread);

        break;
    }

    case DLL_PROCESS_DETACH:
    {
        Globals::CheatWork = false;
        Sleep(100);
        break;
    }
    }

    return TRUE;
}