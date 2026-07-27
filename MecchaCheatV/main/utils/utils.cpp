#include "utils.h"
#include "../sdk/SDK/BP_FirstPersonPlayerState_Online_parameters.hpp"
#include "../sdk/SDK/BP_FirstPersonCharacter_cLeon_Character_Hunter_parameters.hpp"

namespace fs = std::filesystem;

std::string Utils::GetCheatDirectory()
{
    return "C:\\VComDev\\MecchaCheatV";
}

void Utils::CreateCheatDirectory()
{
    try
    {
        const std::string baseDir = "C:\\VComDev";
        const std::string cheatDir = baseDir + "\\MecchaCheatV";
        const std::string configDir = cheatDir + "\\configs";

        if (!fs::exists(baseDir))
        {
            LOG_INFO("Creating base directory");
            fs::create_directory(baseDir);
        }

        if (!fs::exists(cheatDir))
        {
            LOG_INFO("Creating cheat directory");
            fs::create_directory(cheatDir);
        }

        if (!fs::exists(configDir))
        {
            LOG_INFO("Creating config directory");
            fs::create_directory(configDir);
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        LOG_ERROR("Filesystem error: %s", e.what());
    }
}

// Taken from OmegaWare-Framework.
SDK::AGameStateBase* Utils::GetGameStateBase()
{
    SDK::UWorld* pGWorld = SDK::UWorld::GetWorld();
    if (!pGWorld)
        return nullptr;

    SDK::AGameStateBase* pGameState = pGWorld->GameState;
    if (!pGameState)
        return nullptr;

    return pGameState;
}

// Taken from OmegaWare-Framework.
SDK::UGameInstance* Utils::GetGameInstance()
{
    SDK::UWorld* pGWorld = SDK::UWorld::GetWorld();
    if (!pGWorld)
        return nullptr;

    SDK::UGameInstance* pGameInstance = pGWorld->OwningGameInstance;
    if (!pGameInstance)
        return nullptr;

    return pGameInstance;
}

// Taken from OmegaWare-Framework.
SDK::ULocalPlayer* Utils::GetLocalPlayer(int index)
{
    SDK::UGameInstance* pGameInstance = GetGameInstance();
    if (!pGameInstance)
        return nullptr;

    SDK::ULocalPlayer* pLocalPlayer = pGameInstance->LocalPlayers[index];
    if (!pLocalPlayer)
        return nullptr;

    return pLocalPlayer;
}

// Taken from OmegaWare-Framework.
SDK::UGameViewportClient* Utils::GetViewportClient()
{
    SDK::ULocalPlayer* pLocalPlayer = GetLocalPlayer();
    if (!pLocalPlayer)
        return nullptr;

    SDK::UGameViewportClient* pViewportClient = pLocalPlayer->ViewportClient;
    if (!pViewportClient)
        return nullptr;

    return pViewportClient;
}

// Taken from OmegaWare-Framework.
SDK::APlayerController* Utils::GetPlayerController()
{
    SDK::ULocalPlayer* pLocalPlayer = GetLocalPlayer();
    if (!pLocalPlayer)
        return nullptr;

    SDK::APlayerController* pPlayerController = pLocalPlayer->PlayerController;
    if (!pPlayerController)
        return nullptr;

    return pPlayerController;
}

// Taken from OmegaWare-Framework.
SDK::APawn* Utils::GetAcknowledgedPawn()
{
    SDK::APlayerController* pPlayerController = GetPlayerController();
    if (!pPlayerController)
        return nullptr;

    SDK::APawn* pAcknowledgedPawn = pPlayerController->AcknowledgedPawn;
    if (!pAcknowledgedPawn)
        return nullptr;

    return pAcknowledgedPawn;
}

// Taken from OmegaWare-Framework.
SDK::APlayerCameraManager* Utils::GetPlayerCameraManager()
{
    SDK::APlayerController* pPlayerController = GetPlayerController();
    if (!pPlayerController)
        return nullptr;

    SDK::APlayerCameraManager* pPlayerCameraManager = pPlayerController->PlayerCameraManager;
    if (!pPlayerCameraManager)
        return nullptr;

    return pPlayerCameraManager;
}

// Taken from OmegaWare-Framework.
bool Utils::WorldToScreen(SDK::FVector in, SDK::FVector2D& out, bool relative)
{
    SDK::APlayerController* pPlayerController = GetPlayerController();
    if (!pPlayerController)
        return false;

    return pPlayerController->ProjectWorldLocationToScreen(in, &out, relative);
}

// Taken from OmegaWare-Framework.
SDK::FVector2D Utils::W2S(SDK::FVector in, bool relative)
{
    SDK::FVector2D out{ 0.f, 0.f };

    SDK::APlayerController* pPlayerController = GetPlayerController();
    if (!pPlayerController)
        return out;

    pPlayerController->ProjectWorldLocationToScreen(in, &out, relative);

    return out;
}

bool Utils::IsInMatch(SDK::UWorld* World)
{
    auto PC = SDK::UGameplayStatics::GetPlayerController(World, 0);

    return PC
        && PC->K2_GetPawn()
        && World
        && World->GameState;
}

std::string Utils::getKeyName(int keyCode)
{
    switch (keyCode)
    {
    case VK_INSERT: return "INSERT";
    case VK_DELETE: return "DELETE";
    case VK_HOME: return "HOME";
    case VK_END: return "END";
    case VK_PRIOR: return "PAGE UP";
    case VK_NEXT: return "PAGE DOWN";
    case VK_F1: return "F1";
    case VK_F2: return "F2";
    case VK_F3: return "F3";
    case VK_F4: return "F4";
    case VK_F5: return "F5";
    case VK_F6: return "F6";
    case VK_F7: return "F7";
    case VK_F8: return "F8";
    case VK_F9: return "F9";
    case VK_F10: return "F10";
    case VK_F11: return "F11";
    case VK_F12: return "F12";
    case VK_RETURN: return "Enter";
    default:
        if (keyCode >= 'A' && keyCode <= 'Z')
            return std::string(1, (char)keyCode);
        else if (keyCode >= '0' && keyCode <= '9')
            return std::string(1, (char)keyCode);
        else
            return "Key " + std::to_string(keyCode);
    }
}

// Thanks phxgg
bool Utils::isObjectValid(SDK::UObject* obj)
{
    if (obj == nullptr)
        return false;

    uintptr_t addr = reinterpret_cast<uintptr_t>(obj);

    if (addr < 0x10000)
        return false;

    if (addr == UINTPTR_MAX)
        return false;

    __try
    {
        if (obj->Index < 0)
            return false;

        return SDK::UObject::GObjects->GetByIndex(obj->Index) == obj;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}

// thx phxgg
bool Utils::isDead(SDK::AActor* actor)
{
    if (!actor)
        return false;
    if (!isObjectValid(actor))
        return false;
    if (!actor->IsA(SDK::ABP_FirstPersonCharacter_cLeon_Character_C::StaticClass()))
        return false;
    auto character = static_cast<SDK::ABP_FirstPersonCharacter_cLeon_Character_C*>(actor);
    auto mesh = character->Mesh;
    if (!mesh)
        return false;
    if (!isObjectValid(mesh))
        return false;
    return mesh->IsAnySimulatingPhysics();
}
// thx phxgg
void Utils::ChangeName(SDK::APawn* myPlayer, std::string name)
{
    if (name.empty() || !myPlayer || !isObjectValid(myPlayer))
        return;

    auto* myChar = static_cast<SDK::ABP_FirstPersonCharacter_cLeon_Character_C*>(myPlayer);
    if (!myChar)
        return;

    auto* playerState = myChar->PlayerState;
    if (!playerState || !isObjectValid(playerState))
        return;

    if (!playerState->IsA(SDK::ABP_FirstPersonPlayerState_Online_C::StaticClass()))
        return;

    auto* onlineState = static_cast<SDK::ABP_FirstPersonPlayerState_Online_C*>(playerState);

    SDK::UFunction* fn = onlineState->Class->GetFunction("BP_FirstPersonPlayerState_Online_C", "SetName(Server)");
    if (!fn)
        return;

    std::wstring wname;
    int wlen = MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, nullptr, 0);
    if (wlen > 1)
    {
        wname.resize(wlen - 1);
        MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, &wname[0], wlen);
    }

    SDK::Params::BP_FirstPersonPlayerState_Online_C_SetName_Server_ parms{};
    parms.CustomPlayerName_0 = SDK::FString(wname.c_str());
    onlineState->ProcessEvent(fn, &parms);
}
// thx phxgg
bool Utils::isSurvivor(SDK::AActor* actor)
{
    if (!actor) return false;
    auto* baseClass = static_cast<SDK::ABP_FirstPersonCharacter_cLeon_Character_C*>(actor);
    if (!baseClass) return false;
    return baseClass->IsA(SDK::ABP_FirstPersonCharacter_cLeon_Character_Survivor_C::StaticClass());
}
// thx phxgg
bool Utils::isHunter(SDK::AActor* actor)
{
    if (!actor) return false;
    auto* baseClass = static_cast<SDK::ABP_FirstPersonCharacter_cLeon_Character_C*>(actor);
    if (!baseClass) return false;
    return baseClass->IsA(SDK::ABP_FirstPersonCharacter_cLeon_Character_Hunter_C::StaticClass());
}

void Utils::RequestTeleport(const SDK::FVector& location)
{
    MecchaCheatV::Globals::teleportLocation = location;
    MecchaCheatV::Globals::needTeleport = true;
}

void Utils::RequestTeleportToActor(SDK::AActor* target)
{
    if (!target || !isObjectValid(target))
        return;

    RequestTeleport(target->K2_GetActorLocation());
}

void Utils::ProcessTeleport(SDK::APawn* myPlayer)
{
    if (!MecchaCheatV::Globals::needTeleport)
        return;

    MecchaCheatV::Globals::needTeleport = false;

    if (!myPlayer || !isObjectValid(myPlayer))
        return;

    const SDK::FRotator rotation = myPlayer->K2_GetActorRotation();

    myPlayer->K2_TeleportTo(
        MecchaCheatV::Globals::teleportLocation,
        rotation
    );
}