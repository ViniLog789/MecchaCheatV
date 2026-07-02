#include "Includes.h"
#include <TlHelp32.h>
#include "memory.h"

using namespace MecchaCheatV;

bool IsModuleLoaded(const wchar_t* moduleName)
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
    if (snapshot == INVALID_HANDLE_VALUE)
        return false;

    MODULEENTRY32W me{};
    me.dwSize = sizeof(me);

    if (Module32FirstW(snapshot, &me))
    {
        do
        {
            if (_wcsicmp(me.szModule, moduleName) == 0)
            {
                CloseHandle(snapshot);
                return true;
            }
        } while (Module32NextW(snapshot, &me));
    }

    CloseHandle(snapshot);
    return false;
}

void RestartWithDx11Flag()
{
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);

    std::wstring cmd = L"\"";
    cmd += path;
    cmd += L"\" -dx11";

    STARTUPINFOW si{};
    PROCESS_INFORMATION pi{};

    si.cb = sizeof(si);

    CreateProcessW(
        NULL,
        cmd.data(),
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi
    );

    if (pi.hThread) CloseHandle(pi.hThread);
    if (pi.hProcess) CloseHandle(pi.hProcess);
}

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

    if (!IsModuleLoaded(L"d3d11.dll"))
    {
        LOG_WARN("d3d11.dll not found. Restarting in 15 seconds with -dx11...");

        for (int i = 15; i > 0; --i)
        {
            LOG_INFO("Restart in " + std::to_string(i) + " sec");
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        RestartWithDx11Flag();
        FreeLibraryAndExitThread(Globals::globalModule, 0);
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