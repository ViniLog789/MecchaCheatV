#include "Includes.h"
#include <TlHelp32.h>
#include "memory.h"

using namespace MecchaCheatV;

static std::unique_ptr<Logger> loggerInstance;
static std::unique_ptr<Renderer> rendererInstance;
static std::unique_ptr<Hooking> hookingInstance;
static std::unique_ptr<FeatureHandler> featureInstance;

bool IsLikelyFunction(void* address)
{
    if (!address)
        return false;

    MEMORY_BASIC_INFORMATION mbi{};
    if (!VirtualQuery(address, &mbi, sizeof(mbi)))
        return false;

    if (mbi.State != MEM_COMMIT)
        return false;

    DWORD protect = mbi.Protect & 0xFF;

    if (protect != PAGE_EXECUTE &&
        protect != PAGE_EXECUTE_READ &&
        protect != PAGE_EXECUTE_READWRITE &&
        protect != PAGE_EXECUTE_WRITECOPY)
    {
        return false;
    }

    auto* bytes = reinterpret_cast<uint8_t*>(address);

    bool allZero = true;
    bool allCC = true;
    bool allFF = true;

    for (int i = 0; i < 16; i++)
    {
        if (bytes[i] != 0x00) allZero = false;
        if (bytes[i] != 0xCC) allCC = false;
        if (bytes[i] != 0xFF) allFF = false;
    }

    if (allZero || allCC || allFF)
        return false;

    if (bytes[0] == 0x40 ||                // push/pop r*
        bytes[0] == 0x48 ||                // REX
        bytes[0] == 0x55 ||                // push rbp
        bytes[0] == 0x53 ||                // push rbx
        bytes[0] == 0x57 ||                // push rdi
        bytes[0] == 0x41 ||                // REX.B
        bytes[0] == 0xE9 ||                // jmp
        bytes[0] == 0xEB)                  // short jmp
    {
        return true;
    }

    return false;
}

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

        uintptr_t addr = SDK::InSDKUtils::GetImageBase() + SDK::Offsets::ProcessEvent;

        Globals::hookedProcessEvent = reinterpret_cast<ProcessEvent_t>(addr);

        auto* bytes = reinterpret_cast<uint8_t*>(addr);

        std::ostringstream oss;
        oss << std::hex << std::uppercase << std::setfill('0');

        for (int i = 0; i < 16; i++)
            oss << std::setw(2) << (int)bytes[i] << ' '; 

		bool likelyFunction = IsLikelyFunction((void*)addr);
        if (!likelyFunction)
        {
			LOG_ERROR("Failed to verify the ProcessEvent address.\nThe specified address does not appear to point to a valid function.\n\nMecchaCheatV will now unload automatically to prevent a crash."); // for file
			LOG_RELEASE(FOREGROUND_RED, "Failed to verify the ProcessEvent address.\nThe specified address does not appear to point to a valid function.\n\nMecchaCheatV will now unload automatically to prevent a crash."); // for console
            goto failedaddr;
        }
        
        AHK(hookingInstance->OriginalPresent, Hooks::HkPresent); // ADD_HOOK
        AHK(Globals::hookedProcessEvent, Hooks::HkProcessEvent);

        hookingInstance->ApplyHooks();
        hooksApplied = true;

        LOG_RELEASE(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Welcome, to MecchaCheatV\nMeccha Chameleon version: 3.6.0\n");
        LOG_RELEASE(FOREGROUND_BLUE | FOREGROUND_INTENSITY, std::string(32, '-').c_str(), "\n");
        LOG_RELEASE(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Menu navigation:\n");
        LOG_RELEASE(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Press", Utils::getKeyName(MenuToggleKey), "to open / close the menu\n");
        LOG_RELEASE(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Press End to emergency exit cheat\n");
        LOG_RELEASE(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Press Home to reset menu open/close button\n");
        LOG_RELEASE(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Config folder: C:\\VComDev\\MecchaCheatV\\configs\n");
        LOG_RELEASE(FOREGROUND_BLUE | FOREGROUND_INTENSITY, std::string(32, '-').c_str(), "\n");

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

        if (rendererInstance)
            if (ImGui::GetCurrentContext())
                ImGui::SaveIniSettingsToDisk((Utils::GetCheatDirectory() + "\\menu.ini").c_str());
			else LOG_ERROR("ImGui context is null, cannot save menu.ini. Skipping...");
    }
    catch (...) {}

failedaddr:
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