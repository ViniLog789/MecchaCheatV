#pragma once
#include "Includes.h"

namespace Utils
{
    std::string GetCheatDirectory();
    void CreateCheatDirectory();
    SDK::AGameStateBase* GetGameStateBase();
    SDK::UGameInstance* GetGameInstance();
    SDK::ULocalPlayer* GetLocalPlayer(int index = 0);
    SDK::UGameViewportClient* GetViewportClient();
    SDK::APlayerController* GetPlayerController();
    SDK::APawn* GetAcknowledgedPawn();
    SDK::APlayerCameraManager* GetPlayerCameraManager();
    bool WorldToScreen(SDK::FVector in, SDK::FVector2D& out, bool relative = false);
    SDK::FVector2D W2S(SDK::FVector in, bool relative = false);
    bool IsInMatch(SDK::UWorld* World);
    std::string getKeyName(int keyCode);
    bool isObjectValid(SDK::UObject* obj);
    bool isDead(SDK::AActor* actor);
    void ChangeName(SDK::APawn* myPlayer, std::string name);
    bool isSurvivor(SDK::AActor* actor);
    bool isHunter(SDK::AActor* actor);
    void RequestTeleport(const SDK::FVector& location);
    void RequestTeleportToActor(SDK::AActor* target);
    void ProcessTeleport(SDK::APawn* myPlayer);
}